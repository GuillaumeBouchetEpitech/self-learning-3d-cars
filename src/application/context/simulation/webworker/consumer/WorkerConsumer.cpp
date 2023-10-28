

#include "WorkerConsumer.hpp"

#include "application/defines.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/aliasing.hpp"

#include <chrono>
#include <memory> // std::make_unique

#include <emscripten/emscripten.h> // <= emscripten_worker_respond()

namespace {

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMilliSeconds, std::chrono::duration_cast<std::chrono::milliseconds>);

} // namespace

void
WorkerConsumer::processMessage(const char* dataPointer, int dataSize) {
  gero::messaging::MessageView receivedMsg(dataPointer, dataSize);

  int8_t messageType = 0;
  receivedMsg.readInt8(messageType);

  switch (Messages::FromProducer(messageType)) {
  case Messages::FromProducer::LoadWorker: {
    _initializeSimulation(receivedMsg);
    break;
  }

  case Messages::FromProducer::ResetAndProcessSimulation: {
    float elapsedTime;
    uint32_t totalSteps;
    receivedMsg.readFloat(elapsedTime);
    receivedMsg.readUint32(totalSteps);

    _simulationProcess.reset();

    _addNewCars(receivedMsg);
    _processSimulation(elapsedTime, totalSteps);
    break;
  }

  case Messages::FromProducer::ProcessSimulation: {
    float elapsedTime;
    uint32_t totalSteps;
    receivedMsg.readFloat(elapsedTime);
    receivedMsg.readUint32(totalSteps);

    _addNewCars(receivedMsg);
    _processSimulation(elapsedTime, totalSteps);
    break;
  }

  default: {
    D_THROW(std::runtime_error, "unknown worker message");
  }
  }
}

void
WorkerConsumer::_sendBackToProducer() {
  emscripten_worker_respond(const_cast<char*>(_messageToSend.getData()), _messageToSend.getSize());
}

void
WorkerConsumer::_initializeSimulation(gero::messaging::MessageView& receivedMsg) {

  // _historicalTimeData.setSize(5);

  CircuitBuilder::Knots circuitKnots;

  bool isUsingBias = true;
  uint32_t layerInput = 0;
  std::vector<uint32_t> layerHidden;
  uint32_t layerOutput = 0;

  { // read initialisation packet

    //
    // circuit data

    int32_t knotsLength = 0;
    receivedMsg.readVec3(_startTransform.position);
    receivedMsg.readVec4(_startTransform.quaternion);
    receivedMsg.readInt32(knotsLength);

    circuitKnots.reserve(knotsLength); // <= pre-allocate
    for (int32_t ii = 0; ii < knotsLength; ++ii) {
      CircuitBuilder::Knot knot;
      receivedMsg.readVec3(knot.left).readVec3(knot.right);
      receivedMsg.readFloat(knot.size).readVec3(knot.color);
      circuitKnots.emplace_back(knot);
    }

    //
    // neural network data

    receivedMsg.readBoolean(isUsingBias);
    receivedMsg.readUint32(layerInput);
    uint32_t totalHidden = 0;
    receivedMsg.readUint32(totalHidden);
    layerHidden.reserve(totalHidden);
    for (uint32_t ii = 0; ii < totalHidden; ++ii) {
      uint32_t layerValue = 0;
      receivedMsg.readUint32(layerValue);
      layerHidden.emplace_back(layerValue);
    }
    receivedMsg.readUint32(layerOutput);

  } // read initialisation packet

  { // setup neural network topology

    NeuralNetworkTopology neuralNetworkTopology;
    neuralNetworkTopology.init(layerInput, layerHidden, layerOutput, isUsingBias);

    _simulationProcess.initialize(neuralNetworkTopology, _startTransform, circuitKnots);
    _simulationProcess.reset();
    _simulationProcess.getHistoricalTimeData().setSize(20);

  } // setup neural network topology

  { // send reply

    _messageToSend.clear();
    _messageToSend.writeInt8(int8_t(Messages::FromConsumer::WebWorkerLoaded));

    _sendBackToProducer();

  } // send reply
}

void
WorkerConsumer::_addNewCars(gero::messaging::MessageView& receivedMsg) {
  const uint32_t floatWeightsSize = _simulationProcess.getNeuralNetworkTopology().getTotalWeights();
  const uint32_t byteWeightsSize = floatWeightsSize * sizeof(float);

  auto newWeights = std::make_unique<float[]>(floatWeightsSize);
  float* newWeightsRaw = newWeights.get();

  uint32_t totalAgentsToAdd = 0;
  receivedMsg.readUint32(totalAgentsToAdd);

  for (uint32_t ii = 0; ii < totalAgentsToAdd; ++ii) {
    uint32_t dataIndex = 0;
    receivedMsg.readUint32(dataIndex);
    receivedMsg.read(newWeightsRaw, byteWeightsSize);

    _simulationProcess.addNewCar(dataIndex, newWeightsRaw, floatWeightsSize);
  }

  if (totalAgentsToAdd > 0) {
    _simulationProcess.getHistoricalTimeData().reset();
  }
}

void
WorkerConsumer::_processSimulation(float elapsedTime, uint32_t totalSteps) {

  //
  //

  const auto startTime = _getTime();

  //
  //

  for (uint32_t step = 0; step < totalSteps; ++step) {
    _simulationProcess.process(elapsedTime);
  }

  //
  //

  const auto finishTime = _getTime();
  const int32_t delta = _asMilliSeconds(finishTime - startTime).count();

  //
  //

  //
  // compile what to send back
  //

  _messageToSend.clear();
  _messageToSend.writeInt8(int8_t(Messages::FromConsumer::SimulationResult));
  _messageToSend.writeInt32(delta);

  const auto& historicalTimeData = _simulationProcess.getHistoricalTimeData();

  _messageToSend.writeInt32(int32_t(historicalTimeData.getTotalDurations()));
  _messageToSend.writeInt32(int32_t(historicalTimeData.getSize()));
  _messageToSend.writeInt32(int32_t(historicalTimeData.getAverageDuration()));
  _messageToSend.writeInt32(int32_t(historicalTimeData.getMaxDuration()));

  _messageToSend.writeUint32(uint32_t(_simulationProcess.getTotalAgentsAlive()));
  _messageToSend.writeUint32(uint32_t(_simulationProcess.getTotalAgentValues()));

  std::vector<float> tmpNeuronsValues;

  for (std::size_t ii = 0; ii < _simulationProcess.getTotalAgentValues(); ++ii) {

    const auto& currValue = _simulationProcess.getAgentValuesByIndex(ii);
    const auto& currAgent = currValue.carAgent;

    _messageToSend.writeUint64(uint64_t(currValue.dataIndex));

    //
    // core data

    _messageToSend.writeBoolean(currAgent.isAlive());
    _messageToSend.writeBoolean(currAgent.isDying());
    _messageToSend.writeFloat(currAgent.getLife());
    _messageToSend.writeFloat(currAgent.getFitness());
    _messageToSend.writeUint32(currAgent.getTotalUpdates());
    _messageToSend.writeInt32(currAgent.getGroundIndex());

    //
    // transform history

    _messageToSend.writeInt32(int32_t(currValue.transformsHistory.size()));
    for (const auto& transforms : currValue.transformsHistory) {
      _messageToSend.writeVec3(transforms.chassis.position);
      _messageToSend.writeQuat(transforms.chassis.orientation);

      for (const auto& wheel : transforms.wheels) {
        _messageToSend.writeVec3(wheel.position);
        _messageToSend.writeQuat(wheel.orientation);
      }
    }

    //
    //
    //

    if (!currAgent.isAlive())
      continue;

    //
    //
    //

    //
    // spatial data

    const auto chassisBody = currAgent.getBody();
    const auto vehicle = currAgent.getVehicle();

    // record the transformation of the car
    _messageToSend.writeVec3(chassisBody->getPosition());
    _messageToSend.writeQuat(chassisBody->getOrientation());

    // record the transformation of the wheels
    for (int jj = 0; jj < 4; ++jj) {
      _messageToSend.writeVec3(vehicle->getWheelPosition(jj));
      _messageToSend.writeQuat(vehicle->getWheelOrientation(jj));
    }

    _messageToSend.writeVec3(chassisBody->getLinearVelocity());

    //
    // sensor data

    const auto& eyeSensors = currAgent.getEyeSensors();
    for (const auto& sensor : eyeSensors) {
      _messageToSend.writeVec3(sensor.near).writeVec3(sensor.far);
      _messageToSend.writeFloat(sensor.value);
    }

    const auto& gSensor = currAgent.getGroundSensor();
    _messageToSend.writeVec3(gSensor.near).writeVec3(gSensor.far);
    _messageToSend.writeFloat(gSensor.value);

    //
    // neural network data

    tmpNeuronsValues.clear();
    currValue.neuralNet.getNeuronsValues(tmpNeuronsValues);
    _messageToSend.writeInt32(int32_t(tmpNeuronsValues.size()));
    for (float currValue : tmpNeuronsValues) {
      _messageToSend.writeFloat(currValue);
    }
  }

  _simulationProcess.cleanup();

  _sendBackToProducer();
}
