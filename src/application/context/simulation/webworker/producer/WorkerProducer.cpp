
#include "WorkerProducer.hpp"

#include "application/context/simulation/webworker/preprocessing.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

#include "application/defines.hpp"

WorkerProducer::AgentData::AgentData(uint32_t inDataIndex)
  : dataIndex(inDataIndex) {}

WorkerProducer::WorkerProducer(const Definition& def) : _def(def) {
  _workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

  _flags[gero::asValue(Status::WebWorkerLoaded)] = false;
  _flags[gero::asValue(Status::Processing)] = false;
  _flags[gero::asValue(Status::Updated)] = false;

  _waitingAgentsData.reserve(128);
  _allAgentsData.reserve(128);

  { // send initialisation message to worker consumer

    _message.clear();
    _message << int8_t(Messages::FromProducer::LoadWorker);

    //
    // circuit data

    _message << def.startTransform.position;
    _message << def.startTransform.quaternion;
    _message << int(def.knots.size());
    for (const auto& knot : def.knots)
      _message << knot.left << knot.right << knot.size << knot.color;

    //
    // neural network data

    const auto& topology = def.neuralNetworkTopology;
    const auto& hiddenLayers = topology.getHiddenLayers();

    _message << topology.isUsingBias();
    _message << topology.getInputLayerSize();
    _message << int(hiddenLayers.size());
    for (uint32_t layerValue : hiddenLayers)
      _message << layerValue;
    _message << topology.getOutputLayerSize();

    //

    _sendToConsumer();

  } // send intiialisation message to worker consumer
}

void
WorkerProducer::_onMessageCallback(char* dataPointer, int dataSize, void* arg) {
  WorkerProducer* self = static_cast<WorkerProducer*>(arg);

  self->_processMessage(dataPointer, dataSize);
}

void
WorkerProducer::_processMessage(const char* dataPointer, int dataSize) {
  _flags[gero::asValue(Status::Processing)] = false;

  gero::messaging::MessageView receivedMsg(dataPointer, dataSize);

  int8_t messageType = 0;
  receivedMsg >> messageType;

  switch (Messages::FromConsumer(messageType)) {
  case Messages::FromConsumer::WebWorkerLoaded: {
    D_MYLOG("web worker loaded");
    _flags[gero::asValue(Status::WebWorkerLoaded)] = true;
    break;
  }

  case Messages::FromConsumer::SimulationResult: {

    receivedMsg >> _coreState.delta;

    //

    int32_t totalDeltasPerTotalAgents = 0;
    receivedMsg >> totalDeltasPerTotalAgents;
    for (int32_t ii = 0; ii < totalDeltasPerTotalAgents; ++ii) {
      int32_t totalLiveVehicles = 0;
      int32_t stepMilliSeconds = 0;

      receivedMsg >> totalLiveVehicles >> stepMilliSeconds;

      _frameProfiler.set(totalLiveVehicles, stepMilliSeconds);
    }

    //

    receivedMsg >> _coreState.genomesAlive;

    //

    uint32_t totalAgents = 0;
    receivedMsg >> totalAgents;

    for (uint32_t ii = 0; ii < totalAgents; ++ii) {
      //
      // core data

      uint64_t dataIndex = 0;

      receivedMsg >> dataIndex;

      auto it = _agentsDataMap.find(dataIndex);
      if (it == _agentsDataMap.end())
        D_THROW(std::invalid_argument, "data index not found -> " << dataIndex);

      auto& currValue = *it->second;
      auto& currData = currValue.carData;

      const bool carWasAlive = currData.isAlive;

      receivedMsg >> currData.isAlive >> currData.isDying >> currData.life >> currData.fitness >>
        currData.totalUpdates >> currData.groundIndex;

      if (carWasAlive && !currData.isAlive) {
        _currentLiveAgents -= 1;
      }

      //
      // transform history

      int32_t totalHistory = 0;
      receivedMsg >> totalHistory;

      currData.latestTransformsHistory.clear();
      currData.latestTransformsHistory.reserve(totalHistory);

      CarData::CarTransform newData;
      for (int32_t jj = 0; jj < totalHistory; ++jj) {
        receivedMsg >> newData.chassis.position;
        receivedMsg >> newData.chassis.orientation;
        for (auto& currWheel : newData.wheels) {
          receivedMsg >> currWheel.position;
          receivedMsg >> currWheel.orientation;
        }

        currData.latestTransformsHistory.push_back(newData);
      }

      //
      //
      //

      if (!currData.isAlive)
        continue;

      //
      //
      //

      //
      // spatial data

      receivedMsg >> currData.liveTransforms.chassis.position;
      receivedMsg >> currData.liveTransforms.chassis.orientation;
      for (auto& wheelTransform : currData.liveTransforms.wheels) {
        receivedMsg >> wheelTransform.position;
        receivedMsg >> wheelTransform.orientation;
      }

      receivedMsg >> currData.velocity;

      //
      // sensor data

      for (auto& sensor : currData.eyeSensors)
        receivedMsg >> sensor.near >> sensor.far >> sensor.value;

      auto& gSensor = currData.groundSensor;
      receivedMsg >> gSensor.near >> gSensor.far >> gSensor.value;

      //
      // neural network data

      int32_t totalNeuronsValues;
      receivedMsg >> totalNeuronsValues;
      currData.neuronsValues.clear();
      currData.neuronsValues.resize(totalNeuronsValues);
      for (int32_t jj = 0; jj < totalNeuronsValues; ++jj)
        receivedMsg >> currData.neuronsValues.at(jj);
    }

    _flags[gero::asValue(Status::Updated)] = true;
    break;
  }

  default: {
    D_THROW(std::runtime_error, "unknown worker message");
  }
  }
}

void
WorkerProducer::_sendToConsumer() {
  _flags[gero::asValue(Status::Processing)] = true;

  char* dataPointer = const_cast<char*>(_message.getData());
  uint32_t dataSize = _message.getSize();

  em_worker_callback_func callback = WorkerProducer::_onMessageCallback;

  emscripten_call_worker(
    _workerHandle, D_WORKER_MAIN_STR, dataPointer, dataSize, callback,
    (void*)this);
}

void
WorkerProducer::resetAndProcessSimulation(
  float elapsedTime, uint32_t totalSteps
  // ,
  // const NeuralNetworks& neuralNetworks
) {
  _message.clear();
  _message << int8_t(Messages::FromProducer::ResetAndProcessSimulation);
  _message << elapsedTime;
  _message << totalSteps;

  _fillMessageWithAgentToAdd();

  _sendToConsumer();
}

void
WorkerProducer::processSimulation(float elapsedTime, uint32_t totalSteps) {
  _message.clear();
  _message << int8_t(Messages::FromProducer::ProcessSimulation);
  _message << elapsedTime;
  _message << totalSteps;

  _fillMessageWithAgentToAdd();

  _sendToConsumer();
}

void
WorkerProducer::_fillMessageWithAgentToAdd() {
  const uint32_t totalAgentsToAdd = _waitingAgentsData.size();
  _message << totalAgentsToAdd;

  for (const auto currAgent : _waitingAgentsData) {
    _message << currAgent->dataIndex;

    const auto& weights = currAgent->connectionsWeights;
    _message.append(weights.data(), weights.size() * sizeof(float));

    _allAgentsData.push_back(currAgent);
    // _agentsDataMap[currAgent->dataIndex] = currAgent;

    _currentLiveAgents += 1;
  }

  _waitingAgentsData.clear();
}

bool
WorkerProducer::addNewAgent(
  uint32_t inDataIndex, const AbstractGenome& inGenome) {
  const uint32_t totalLiveVehicles = getTotalLiveAgents();

  if (totalLiveVehicles > 20) {
    if (_waitingAgentsData.size() >= 10)
      return false;

    if (_frameProfiler.getMaxDelta(totalLiveVehicles) >= 30)
      return false;
  }

  auto newAgent = std::make_shared<AgentData>(inDataIndex);

  // TODO: copy/realloc (-_-)
  newAgent->connectionsWeights = inGenome.getConnectionsWeights();

  _waitingAgentsData.push_back(newAgent);
  _agentsDataMap[newAgent->dataIndex] = newAgent;

  return true;
}

void
WorkerProducer::cleanupDeadAgents() {
  for (std::size_t index = 0; index < _allAgentsData.size();) {
    auto& currData = _allAgentsData.at(index);

    if (!currData->carData.isAlive) {
      _agentsDataMap.erase(currData->dataIndex);

      // fast removal (no reallocation)
      if (index + 1 < _allAgentsData.size())
        std::swap(currData, _allAgentsData.back());
      _allAgentsData.pop_back();
    } else {
      ++index;
    }
  }
}

bool
WorkerProducer::isLoaded() const {
  return _flags[gero::asValue(Status::WebWorkerLoaded)];
}

bool
WorkerProducer::isProcessing() const {
  return _flags[gero::asValue(Status::Processing)];
}

bool
WorkerProducer::isUpdated() const {
  return _flags[gero::asValue(Status::Updated)];
}

// const AllCarsData&
// WorkerProducer::getCarsData() const {
//   return _allAgentsData;
// }

const CarData&
WorkerProducer::getCarDataByDataIndex(uint32_t inDataIndex) const {
  auto it = _agentsDataMap.find(inDataIndex);
  if (it == _agentsDataMap.end())
    D_THROW(
      std::invalid_argument, "data index not found, value -> " << inDataIndex);

  return it->second->carData;
}

const CarData&
WorkerProducer::getCarDataByIndex(uint32_t inIndex) const {
  return _allAgentsData.at(inIndex)->carData;
}

std::size_t
WorkerProducer::getTotalCarsData() const {
  return _allAgentsData.size();
}

const AbstractSimulation::CoreState&
WorkerProducer::getCoreState() const {
  return _coreState;
}

uint32_t
WorkerProducer::getTotalLiveAgents() const {
  return _currentLiveAgents + uint32_t(_waitingAgentsData.size());
}
