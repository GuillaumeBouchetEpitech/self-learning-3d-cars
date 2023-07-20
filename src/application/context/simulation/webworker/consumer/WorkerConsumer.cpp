

#include "WorkerConsumer.hpp"

#include "../../common.hpp"

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

WorkerConsumer::AgentValues::AgentValues(uint32_t inDataIndex, const NeuralNetworkTopology& inNeuralNetworkTopology)
  : dataIndex(inDataIndex), neuralNet(inNeuralNetworkTopology) {
  transformsHistory.reserve(50); // TODO: hardcoded
}

void
WorkerConsumer::processMessage(const char* dataPointer, int dataSize) {
  gero::messaging::MessageView receivedMsg(dataPointer, dataSize);

  int8_t messageType = 0;
  receivedMsg >> messageType;

  switch (Messages::FromProducer(messageType)) {
  case Messages::FromProducer::LoadWorker: {
    _initializeSimulation(receivedMsg);
    break;
  }

  case Messages::FromProducer::ResetAndProcessSimulation: {
    float elapsedTime;
    uint32_t totalSteps;
    receivedMsg >> elapsedTime >> totalSteps;

    _resetPhysic();
    _addNewCars(receivedMsg);
    _processSimulation(elapsedTime, totalSteps);
    break;
  }

  case Messages::FromProducer::ProcessSimulation: {
    float elapsedTime;
    uint32_t totalSteps;
    receivedMsg >> elapsedTime >> totalSteps;

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

  _historicalTimeData.setSize(5);

  CircuitBuilder::Knots circuitKnots;

  bool isUsingBias = true;
  uint32_t layerInput = 0;
  std::vector<uint32_t> layerHidden;
  uint32_t layerOutput = 0;

  { // read initialisation packet

    //
    // circuit data

    int32_t knotsLength = 0;
    receivedMsg >> _startTransform.position;
    receivedMsg >> _startTransform.quaternion;
    receivedMsg >> knotsLength;
    circuitKnots.reserve(knotsLength); // <= pre-allocate
    for (int32_t ii = 0; ii < knotsLength; ++ii) {
      CircuitBuilder::Knot knot;
      receivedMsg >> knot.left >> knot.right >> knot.size >> knot.color;
      circuitKnots.emplace_back(knot);
    }

    //
    // neural network data

    receivedMsg >> isUsingBias;
    receivedMsg >> layerInput;
    uint32_t totalHidden = 0;
    receivedMsg >> totalHidden;
    layerHidden.reserve(totalHidden);
    for (uint32_t ii = 0; ii < totalHidden; ++ii) {
      uint32_t layerValue = 0;
      receivedMsg >> layerValue;
      layerHidden.emplace_back(layerValue);
    }
    receivedMsg >> layerOutput;

  } // read initialisation packet

  { // generate circuit

    _circuitBuilder.load(_startTransform, circuitKnots);

    _resetPhysic();

  } // generate circuit

  { // setup neural network topology

    _neuralNetworkTopology.init(layerInput, layerHidden, layerOutput, isUsingBias);

  } // setup neural network topology

  _allAgentValues.reserve(256); // TODO: hardcoded

  { // send reply

    _messageToSend.clear();
    _messageToSend << int8_t(Messages::FromConsumer::WebWorkerLoaded);

    _sendBackToProducer();

  } // send reply
}

void
WorkerConsumer::_addNewCars(gero::messaging::MessageView& receivedMsg) {
  const uint32_t floatWeightsSize = _neuralNetworkTopology.getTotalWeights();
  const uint32_t byteWeightsSize = floatWeightsSize * sizeof(float);

  auto newWeights = std::make_unique<float[]>(floatWeightsSize);
  float* newWeightsRaw = newWeights.get();

  uint32_t totalAgentsToAdd = 0;
  receivedMsg >> totalAgentsToAdd;

  for (uint32_t ii = 0; ii < totalAgentsToAdd; ++ii) {
    uint32_t dataIndex = 0;
    receivedMsg >> dataIndex;
    receivedMsg.read(newWeightsRaw, byteWeightsSize);

    auto newValues = std::make_shared<AgentValues>(dataIndex, _neuralNetworkTopology);
    newValues->neuralNet.setConnectionsWeights(newWeightsRaw, floatWeightsSize);
    newValues->carAgent.reset(_physicWorld.get(), _startTransform.position, _startTransform.quaternion);

    _allAgentValues.emplace_back(newValues);
  }

  // _historicalTimeData.reset();
}

void
WorkerConsumer::_processSimulation(float elapsedTime, uint32_t totalSteps) {
  // update the simulation

  const auto startTime = _getTime();

  //
  // run all agent(s)
  //

  for (auto& currValues : _allAgentValues)
    currValues->transformsHistory.clear();

  CarData::CarTransform tmpCarTransform;

  for (uint32_t step = 0; step < totalSteps; ++step) {
    _historicalTimeData.start();

    constexpr uint32_t maxSubSteps = 0;
    _physicWorld->step(elapsedTime, maxSubSteps, elapsedTime);

    for (auto currValues : _allAgentValues) {
      auto& carAgent = currValues->carAgent;

      if (!carAgent.isAlive())
        continue;

      carAgent.update(elapsedTime, currValues->neuralNet);

      //
      // record (updated) spatial data
      //

      {
        const auto body = carAgent.getBody();
        const auto vehicle = carAgent.getVehicle();

        // transformation matrix of the car
        tmpCarTransform.chassis.position = body->getPosition();
        tmpCarTransform.chassis.orientation = body->getOrientation();

        // transformation matrix of the wheels
        for (uint32_t jj = 0; jj < 4U; ++jj) {
          auto& currWheel = tmpCarTransform.wheels.at(jj);

          currWheel.position = vehicle->getWheelPosition(jj);
          currWheel.orientation = vehicle->getWheelOrientation(jj);
        }

        currValues->transformsHistory.push_back(tmpCarTransform);
      }
    }

    _historicalTimeData.stop();
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
  _messageToSend << int8_t(Messages::FromConsumer::SimulationResult);
  _messageToSend << delta;

  const bool isReadyToAddMoreCars = _historicalTimeData.getTotalDurations() < _historicalTimeData.getSize();
  _messageToSend << isReadyToAddMoreCars;
  _messageToSend << int32_t(_historicalTimeData.getMaxDuration());

  uint32_t genomesAlive = 0;
  for (auto currValues : _allAgentValues)
    if (currValues->carAgent.isAlive())
      ++genomesAlive;

  _messageToSend << genomesAlive;
  _messageToSend << uint32_t(_allAgentValues.size());

  std::vector<float> tmpNeuronsValues;

  for (const auto currValue : _allAgentValues) {
    _messageToSend << currValue->dataIndex;

    const auto& currAgent = currValue->carAgent;

    //
    // core data

    _messageToSend << currValue->dataIndex;
    _messageToSend << currAgent.isAlive() << currAgent.isDying();
    _messageToSend << currAgent.getLife() << currAgent.getFitness();
    _messageToSend << currAgent.getTotalUpdates() << currAgent.getGroundIndex();

    //
    // transform history

    _messageToSend << int32_t(currValue->transformsHistory.size());
    for (const auto& transforms : currValue->transformsHistory) {
      _messageToSend << transforms.chassis.position;
      _messageToSend << transforms.chassis.orientation;

      for (const auto& wheel : transforms.wheels) {
        _messageToSend << wheel.position;
        _messageToSend << wheel.orientation;
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

    const auto body = currAgent.getBody();
    const auto vehicle = currAgent.getVehicle();

    // record the transformation of the car
    _messageToSend << body->getPosition();
    _messageToSend << body->getOrientation();

    // record the transformation of the wheels
    for (int jj = 0; jj < 4; ++jj) {
      _messageToSend << vehicle->getWheelPosition(jj);
      _messageToSend << vehicle->getWheelOrientation(jj);
    }

    _messageToSend << body->getLinearVelocity();

    //
    // sensor data

    const auto& eyeSensors = currAgent.getEyeSensors();
    for (const auto& sensor : eyeSensors)
      _messageToSend << sensor.near << sensor.far << sensor.value;

    const auto& gSensor = currAgent.getGroundSensor();
    _messageToSend << gSensor.near << gSensor.far << gSensor.value;

    //
    // neural network data

    tmpNeuronsValues.clear();
    currValue->neuralNet.getNeuronsValues(tmpNeuronsValues);
    _messageToSend << int32_t(tmpNeuronsValues.size());
    for (float currValue : tmpNeuronsValues)
      _messageToSend << currValue;
  }

  //
  // dead agent(s) cleanup
  //

  for (std::size_t index = 0; index < _allAgentValues.size();) {
    if (!_allAgentValues.at(index)->carAgent.isAlive()) {
      // fast removal (no reallocation)
      if (index + 1 < _allAgentValues.size())
        std::swap(_allAgentValues.at(index), _allAgentValues.back());
      _allAgentValues.pop_back();
    } else {
      ++index;
    }
  }

  _sendBackToProducer();
}

void
WorkerConsumer::_resetPhysic() {
  _physicWorld = std::make_unique<gero::physics::PhysicWorld>();

  { // generate circuit

    int groundIndex = 0;

    auto onNewGroundPatch = [&](
                              const CircuitBuilder::Vec3Array& vertices, const CircuitBuilder::Vec3Array& colors,
                              const CircuitBuilder::Vec3Array& normals,
                              const CircuitBuilder::Indices& indices) -> void {
      static_cast<void>(colors);  // <= unused
      static_cast<void>(normals); // <= unused

      gero::physics::PhysicShapeDef shapeDef;
      shapeDef.type = gero::physics::PhysicShapeDef::Type::staticMesh;
      shapeDef.data.staticMesh.verticesData = const_cast<glm::vec3*>(vertices.data());
      shapeDef.data.staticMesh.verticesLength = vertices.size();
      shapeDef.data.staticMesh.indicesData = const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
      shapeDef.data.staticMesh.indicesLength = indices.size();

      gero::physics::PhysicBodyDef bodyDef;
      bodyDef.shape = shapeDef;
      bodyDef.mass = 0.0f;
      bodyDef.group = gero::asValue(Groups::ground);
      bodyDef.mask = gero::asValue(Masks::ground);

      auto body = _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
      body->setFriction(1.0f);
      body->setUserValue(groundIndex++);
    };

    auto onNewWallPatch = [&](
                            const CircuitBuilder::Vec3Array& vertices, const CircuitBuilder::Vec3Array& colors,
                            const CircuitBuilder::Vec3Array& normals, const CircuitBuilder::Indices& indices) -> void {
      static_cast<void>(colors);  // <= unused
      static_cast<void>(normals); // <= unused

      gero::physics::PhysicShapeDef shapeDef;
      shapeDef.type = gero::physics::PhysicShapeDef::Type::staticMesh;
      shapeDef.data.staticMesh.verticesData = const_cast<glm::vec3*>(vertices.data());
      shapeDef.data.staticMesh.verticesLength = vertices.size();
      shapeDef.data.staticMesh.indicesData = const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
      shapeDef.data.staticMesh.indicesLength = indices.size();

      gero::physics::PhysicBodyDef bodyDef;
      bodyDef.shape = shapeDef;
      bodyDef.mass = 0.0f;
      bodyDef.group = gero::asValue(Groups::wall);
      bodyDef.mask = gero::asValue(Masks::wall);

      _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
    };

    _circuitBuilder.generateCircuitGeometry(onNewGroundPatch, onNewWallPatch);

  } // generate circuit

  { // generate floor

    gero::physics::PhysicShapeDef shapeDef;
    shapeDef.type = gero::physics::PhysicShapeDef::Type::box;
    shapeDef.data.box.size = {1000, 1000, 0.5f};

    gero::physics::PhysicBodyDef bodyDef;
    bodyDef.shape = shapeDef;
    bodyDef.group = gero::asValue(Groups::ground);
    bodyDef.mask = gero::asValue(Masks::ground);
    bodyDef.mass = 0.0f;

    auto body = _physicWorld->getPhysicBodyManager().createAndAddBody(bodyDef);
    body->setPosition({0.0f, 0.0f, -0.5f});

  } // generate floor
}
