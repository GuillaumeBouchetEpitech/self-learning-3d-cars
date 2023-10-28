
#include "WorkerProducer.hpp"

#include "application/context/simulation/webworker/preprocessing.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

#include "application/defines.hpp"

WorkerProducer::AgentData::AgentData(uint32_t inDataIndex) : dataIndex(inDataIndex) {}

WorkerProducer::WorkerProducer(const Definition& def) : _def(def) {
  _workerHandle = emscripten_create_worker(D_WORKER_SCRIPT_URL);

  _flags[gero::asValue(Status::WebWorkerLoaded)] = false;
  _flags[gero::asValue(Status::Processing)] = false;
  _flags[gero::asValue(Status::Updated)] = false;

  _waitingAgentsData.reserve(128);
  _allAgentsData.reserve(128);

  { // send initialisation message to worker consumer

    _message.clear();
    _message.writeInt8(int8_t(Messages::FromProducer::LoadWorker));

    //
    // circuit data

    _message.writeVec3(def.startTransform.position);
    _message.writeVec4(def.startTransform.quaternion);


    _message.writeInt32(int32_t(def.knots.size()));
    for (const auto& knot : def.knots)
    {
      _message.writeVec3(knot.left);
      _message.writeVec3(knot.right);
      _message.writeFloat(knot.size);
      _message.writeVec3(knot.color);
    }

    //
    // neural network data

    const auto& topology = def.neuralNetworkTopology;
    const auto& hiddenLayers = topology.getHiddenLayers();

    _message.writeBoolean(topology.isUsingBias());
    _message.writeUint32(topology.getInputLayerSize());

    _message.writeUint32(uint32_t(hiddenLayers.size()));
    for (uint32_t layerValue : hiddenLayers)
    {
      _message.writeUint32(layerValue);
    }
    _message.writeUint32(topology.getOutputLayerSize());

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
  receivedMsg.readInt8(messageType);

  switch (Messages::FromConsumer(messageType)) {
  case Messages::FromConsumer::WebWorkerLoaded: {
    D_MYLOG("web worker loaded");
    _flags[gero::asValue(Status::WebWorkerLoaded)] = true;
    break;
  }

  case Messages::FromConsumer::SimulationResult: {

    receivedMsg.readUint32(_coreState.delta);

    //

    receivedMsg.readInt32(_currSizeDurations);
    receivedMsg.readInt32(_maxSizeDurations);
    receivedMsg.readInt32(_averageDuration);
    receivedMsg.readInt32(_maxDuration);

    //

    receivedMsg.readUint32(_coreState.genomesAlive);

    //

    uint32_t totalAgents = 0;
    receivedMsg.readUint32(totalAgents);

    for (uint32_t ii = 0; ii < totalAgents; ++ii) {

      //
      // core data

      uint64_t dataIndex = 0;

      receivedMsg.readUint64(dataIndex);

      auto it = _agentsDataMap.find(dataIndex);
      if (it == _agentsDataMap.end())
        D_THROW(std::invalid_argument, "data index not found -> " << dataIndex);

      auto& currValue = *it->second;
      auto& currData = currValue.carData;

      const bool carWasAlive = currData.isAlive;

      receivedMsg.readBoolean(currData.isAlive);
      receivedMsg.readBoolean(currData.isDying);
      receivedMsg.readFloat(currData.life);
      receivedMsg.readFloat(currData.fitness);
      receivedMsg.readUint32(currData.totalUpdates);
      receivedMsg.readInt32(currData.groundIndex);

      if (carWasAlive && !currData.isAlive) {
        _currentLiveAgents -= 1;
      }

      //
      // transform history

      int32_t totalHistory = 0;
      receivedMsg.readInt32(totalHistory);

      currData.latestTransformsHistory.clear();
      currData.latestTransformsHistory.reserve(totalHistory);

      CarData::CarTransform newData;
      for (int32_t jj = 0; jj < totalHistory; ++jj) {
        receivedMsg.readVec3(newData.chassis.position);
        receivedMsg.readQuat(newData.chassis.orientation);
        for (auto& currWheel : newData.wheels) {
          receivedMsg.readVec3(currWheel.position);
          receivedMsg.readQuat(currWheel.orientation);
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

      receivedMsg.readVec3(currData.liveTransforms.chassis.position);
      receivedMsg.readQuat(currData.liveTransforms.chassis.orientation);
      for (auto& wheelTransform : currData.liveTransforms.wheels) {
        receivedMsg.readVec3(wheelTransform.position);
        receivedMsg.readQuat(wheelTransform.orientation);
      }

      receivedMsg.readVec3(currData.velocity);

      //
      // sensor data

      for (auto& sensor : currData.eyeSensors) {
        receivedMsg.readVec3(sensor.near);
        receivedMsg.readVec3(sensor.far);
        receivedMsg.readFloat(sensor.value);
      }

      auto& gSensor = currData.groundSensor;
      receivedMsg.readVec3(gSensor.near);
      receivedMsg.readVec3(gSensor.far);
      receivedMsg.readFloat(gSensor.value);

      //
      // neural network data

      int32_t totalNeuronsValues;
      receivedMsg.readInt32(totalNeuronsValues);

      currData.neuronsValues.clear();
      currData.neuronsValues.resize(totalNeuronsValues);
      for (int32_t jj = 0; jj < totalNeuronsValues; ++jj) {
        receivedMsg.readFloat(currData.neuronsValues.at(jj));
      }
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

  emscripten_call_worker(_workerHandle, D_WORKER_MAIN_STR, dataPointer, dataSize, callback, (void*)this);
}

void
WorkerProducer::resetAndProcessSimulation(
  float elapsedTime, uint32_t totalSteps
  // ,
  // const NeuralNetworks& neuralNetworks
) {
  _message.clear();
  _message.writeInt8(int8_t(Messages::FromProducer::ResetAndProcessSimulation));
  _message.writeFloat(elapsedTime);
  _message.writeUint32(totalSteps);

  _fillMessageWithAgentToAdd();

  _sendToConsumer();
}

void
WorkerProducer::processSimulation(float elapsedTime, uint32_t totalSteps) {
  _message.clear();
  _message.writeInt8(int8_t(Messages::FromProducer::ProcessSimulation));
  _message.writeFloat(elapsedTime);
  _message.writeUint32(totalSteps);

  _fillMessageWithAgentToAdd();

  _sendToConsumer();
}

void
WorkerProducer::_fillMessageWithAgentToAdd() {
  const uint32_t totalAgentsToAdd = _waitingAgentsData.size();

  _message.writeUint32(totalAgentsToAdd);

  for (const auto currAgent : _waitingAgentsData) {
    _message.writeUint32(currAgent->dataIndex);

    const auto& weights = currAgent->connectionsWeights;
    _message.append(weights.data(), weights.size() * sizeof(float));

    _allAgentsData.push_back(currAgent);
    // _agentsDataMap[currAgent->dataIndex] = currAgent;

    _currentLiveAgents += 1;
  }

  _waitingAgentsData.clear();
}

bool
WorkerProducer::addNewAgent(uint32_t inDataIndex, const AbstractGenome& inGenome) {

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

const CarData&
WorkerProducer::getCarDataByDataIndex(uint32_t inDataIndex) const {
  auto it = _agentsDataMap.find(inDataIndex);
  if (it == _agentsDataMap.end())
    D_THROW(std::invalid_argument, "data index not found, value -> " << inDataIndex);

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

uint32_t
WorkerProducer::getWaitingAgents() const {
  return uint32_t(_waitingAgentsData.size());
}

int32_t WorkerProducer::getCurrSizeDuration() const
{
  return _currSizeDurations;
}
int32_t WorkerProducer::getMaxSizeDuration() const
{
  return _maxSizeDurations;
}
int32_t WorkerProducer::getAverageDuration() const
{
  return _averageDuration;
}
int32_t WorkerProducer::getMaxDuration() const
{
  return _maxDuration;
}

void WorkerProducer::resetDurations()
{
  _currSizeDurations = 0;
  _maxSizeDurations = 0;
  _averageDuration = 0;
  _maxDuration = 0;
}
