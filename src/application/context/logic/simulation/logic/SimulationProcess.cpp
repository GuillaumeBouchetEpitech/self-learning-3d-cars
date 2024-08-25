
#include "SimulationProcess.hpp"

#include "common.hpp"

#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/aliasing.hpp"
#include "geronimo/system/asValue.hpp"

namespace {

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMilliSeconds, std::chrono::duration_cast<std::chrono::milliseconds>);

} // namespace

SimulationProcess::AgentValues::AgentValues(uint32_t inDataIndex, const NeuralNetworkTopology& inNeuralNetworkTopology)
  : dataIndex(inDataIndex), neuralNet(inNeuralNetworkTopology) {
  transformsHistory.reserve(50); // TODO: hardcoded
}

//
//
//

void
SimulationProcess::initialize(
  const NeuralNetworkTopology& inNeuralNetworkTopology, const CircuitBuilder::StartTransform& inStartTransform,
  const CircuitBuilder::Knots& inCircuitKnots) {
  _neuralNetworkTopology = inNeuralNetworkTopology;

  _startTransform = inStartTransform;

  _circuitBuilder.load(inStartTransform, inCircuitKnots);
  reset();

  _allAgentValues.reserve(256); // TODO: hardcoded
}

void
SimulationProcess::reset() {
  _allAgentValues.clear();

  _physicWorld = gero::physics::AbstractPhysicWorld::create();

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
      body->setFriction(0.0f);
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

    _circuitBuilder.generateCircuitGeometry(onNewGroundPatch, onNewWallPatch, onNewWallPatch);

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

void
SimulationProcess::process(float elapsedTime) {
  CarData::CarTransform tmpCarTransform;

  _historicalTimeData.start();

  constexpr uint32_t maxSubSteps = 0;
  _physicWorld->step(elapsedTime, maxSubSteps, elapsedTime);

  for (auto currValues : _allAgentValues) {
    auto& carAgent = currValues->carAgent;

    if (!carAgent.isAlive()) {
      continue;
    }

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

void
SimulationProcess::addNewCar(uint32_t dataIndex, const float* inWeightsData, std::size_t inWeightsLength) {
  auto newValues = std::make_shared<AgentValues>(dataIndex, _neuralNetworkTopology);
  newValues->neuralNet.setConnectionsWeights(inWeightsData, inWeightsLength);
  newValues->carAgent.reset(_physicWorld.get(), _startTransform.position, _startTransform.quaternion, _startTransform.linearVelocity);

  _allAgentValues.emplace_back(newValues);
}

void
SimulationProcess::cleanup() {
  for (std::size_t index = 0; index < _allAgentValues.size();) {
    auto& currAgent = _allAgentValues.at(index);

    if (!currAgent->carAgent.isAlive()) {
      // fast removal (no reallocation)

      if (index + 1 < _allAgentValues.size()) {
        std::swap(currAgent, _allAgentValues.back());
      }

      _allAgentValues.pop_back();
    } else {

      currAgent->transformsHistory.clear();

      ++index;
    }
  }
}

const NeuralNetworkTopology&
SimulationProcess::getNeuralNetworkTopology() const {
  return _neuralNetworkTopology;
}

const SimulationProcess::AgentValues&
SimulationProcess::getAgentValuesByIndex(std::size_t index) const {
  return *_allAgentValues.at(index);
}
std::size_t
SimulationProcess::getTotalAgentValues() const {
  return _allAgentValues.size();
}

std::size_t
SimulationProcess::getTotalAgentsAlive() const {
  std::size_t totalAlive = 0;
  for (auto currValues : _allAgentValues)
    if (currValues->carAgent.isAlive())
      ++totalAlive;
  return totalAlive;
}

gero::metrics::HistoricalTimeData&
SimulationProcess::getHistoricalTimeData() {
  return _historicalTimeData;
}
const gero::metrics::HistoricalTimeData&
SimulationProcess::getHistoricalTimeData() const {
  return _historicalTimeData;
}
