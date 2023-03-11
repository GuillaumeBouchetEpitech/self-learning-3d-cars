
#include "PthreadSimulation.hpp"

#include "application/defines.hpp"

#include "../common.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/aliasing.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <chrono>

namespace {

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMilliSeconds, std::chrono::duration_cast<std::chrono::milliseconds>);

} // namespace

PthreadSimulation::AgentValues::AgentValues(
  uint64_t inDataIndex,
  const NeuralNetworkTopology& inNeuralNetworkTopology)
  : dataIndex(inDataIndex)
  , neuralNet(inNeuralNetworkTopology)
{}

PthreadSimulation::~PthreadSimulation() {
  // the threads must be stopped before anything else is destroyed
  _multithreadProducer.quit();
}

void
PthreadSimulation::initialize(const Definition& def) {

  if (def.totalGenomes == 0)
    D_THROW(
      std::invalid_argument, "received invalid number of total genomes"
                               << ", input=" << def.totalGenomes);

  if (def.totalCores == 0)
    D_THROW(
      std::invalid_argument, "received invalid number of cores"
                               << ", input=" << def.totalCores);

  if (!def.neuralNetworkTopology.isValid())
    D_THROW(std::invalid_argument, "received invalid neural network topology");

  _def = def;

  GeneticAlgorithm::Definition genAlgoDef;
  genAlgoDef.topology = _def.neuralNetworkTopology;
  genAlgoDef.totalGenomes = _def.totalGenomes;
  genAlgoDef.minimumMutations = 0;
  genAlgoDef.mutationMaxChance = 0.2f;
  genAlgoDef.mutationMaxEffect = 0.2f;

  gero::rng::RNG::ensureRandomSeed();

  genAlgoDef.getRandomCallback = []()
  {
    return gero::rng::RNG::getNormalisedValue();
  };

  _geneticAlgorithm.initialize(genAlgoDef);

#if defined D_WEB_BUILD
  // blocking on the main thread of the browser is not advised...
  constexpr bool k_avoidBlocking = true;
#else
  constexpr bool k_avoidBlocking = false;
#endif

  _multithreadProducer.initialize(_def.totalCores, k_avoidBlocking);

  _coreStates.resize(_def.totalCores);

  {

    auto onNewPhysicGroundPatch =
      [&](
        const CircuitBuilder::Vec3Array& vertices,
        const CircuitBuilder::Vec3Array& colors,
        const CircuitBuilder::Vec3Array& normals,
        const CircuitBuilder::Indices& indices) -> void {
      if (_def.onNewGroundPatch)
        _def.onNewGroundPatch(vertices, colors, normals, indices);
    };

    auto onNewPhysicWallPatch =
      [&](
        const CircuitBuilder::Vec3Array& vertices,
        const CircuitBuilder::Vec3Array& colors,
        const CircuitBuilder::Vec3Array& normals,
        const CircuitBuilder::Indices& indices) -> void {
      if (_def.onNewWallPatch)
        _def.onNewWallPatch(vertices, colors, normals, indices);
    };

    _circuitBuilder.parse(_def.filename);
    _circuitBuilder.generateWireFrameSkeleton(_def.onSkeletonPatch);
    _circuitBuilder.generateCircuitGeometry(
      onNewPhysicGroundPatch, onNewPhysicWallPatch);
    _startTransform = _circuitBuilder.getStartTransform();
  }

  _resetPhysic();

  _allAgentValues.reserve(_def.totalGenomes);

  _carsData.resize(_def.totalGenomes);
  for (auto& carData : _carsData)
    carData.latestTransformsHistory.reserve(50); // TODO: hardcoded (o_o)

  _addCars();
}

void
PthreadSimulation::_resetPhysic() {
  _allThreadsData.clear();
  _allThreadsData.reserve(_def.totalCores);
  for (unsigned int ii = 0; ii < _def.totalCores; ++ii)
  {
    auto newData = std::make_unique<ThreadData>();
    auto& bManager = newData->physicWorld.getPhysicBodyManager();

    { // generate circuit

      int groundIndex = 0;
      auto onNewPhysicGroundPatch =
        [&](
          const CircuitBuilder::Vec3Array& vertices,
          const CircuitBuilder::Vec3Array& colors,
          const CircuitBuilder::Vec3Array& normals,
          const CircuitBuilder::Indices& indices) -> void {
        static_cast<void>(colors);  // <= unused
        static_cast<void>(normals); // <= unused

        gero::physics::PhysicShapeDef shapeDef;
        shapeDef.type = gero::physics::PhysicShapeDef::Type::staticMesh;
        shapeDef.data.staticMesh.verticesData =
          const_cast<glm::vec3*>(vertices.data());
        shapeDef.data.staticMesh.verticesLength = vertices.size();
        shapeDef.data.staticMesh.indicesData =
          const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
        shapeDef.data.staticMesh.indicesLength = indices.size();

        gero::physics::PhysicBodyDef bodyDef;
        bodyDef.shape = shapeDef;
        bodyDef.mass = 0.0f;
        bodyDef.group = gero::asValue(Groups::ground);
        bodyDef.mask = gero::asValue(Masks::ground);

        auto body = bManager.createAndAddBody(bodyDef);
        body->setFriction(1.0f);
        body->setUserValue(groundIndex);

        groundIndex++;
      };

      auto onNewPhysicWallPatch =
        [&](
          const CircuitBuilder::Vec3Array& vertices,
          const CircuitBuilder::Vec3Array& colors,
          const CircuitBuilder::Vec3Array& normals,
          const CircuitBuilder::Indices& indices) -> void {
        static_cast<void>(colors);  // <= unused
        static_cast<void>(normals); // <= unused

        gero::physics::PhysicShapeDef shapeDef;
        shapeDef.type = gero::physics::PhysicShapeDef::Type::staticMesh;
        shapeDef.data.staticMesh.verticesData =
          const_cast<glm::vec3*>(vertices.data());
        shapeDef.data.staticMesh.verticesLength = vertices.size();
        shapeDef.data.staticMesh.indicesData =
          const_cast<int32_t*>(static_cast<const int32_t*>(indices.data()));
        shapeDef.data.staticMesh.indicesLength = indices.size();

        gero::physics::PhysicBodyDef bodyDef;
        bodyDef.shape = shapeDef;
        bodyDef.mass = 0.0f;
        bodyDef.group = gero::asValue(Groups::wall);
        bodyDef.mask = gero::asValue(Masks::wall);

        bManager.createAndAddBody(bodyDef);
      };

      _circuitBuilder.generateCircuitGeometry(
        onNewPhysicGroundPatch, onNewPhysicWallPatch);

    } // generate circuit

    { // floor

      gero::physics::PhysicShapeDef shapeDef;
      shapeDef.type = gero::physics::PhysicShapeDef::Type::box;
      shapeDef.data.box.size = {1000, 1000, 0.5f};

      gero::physics::PhysicBodyDef bodyDef;
      bodyDef.shape = shapeDef;
      bodyDef.mass = 0.0f;
      bodyDef.group = gero::asValue(Groups::ground);
      bodyDef.mask = gero::asValue(Masks::ground);

      auto body = bManager.createAndAddBody(bodyDef);
      body->setPosition({0.0f, 0.0f, -0.5f});

    } // floor

    _allThreadsData.emplace_back(std::move(newData));
  }
}

void
PthreadSimulation::update(float elapsedTime, unsigned int totalSteps) {

  if (!_multithreadProducer.allCompleted())
    return;

  //
  //
  //

  _updateCarResult();

  if (_isFirstGenerationFrame) {
    _isFirstGenerationFrame = false;

    if (_callbacks.onResetAndProcess)
      _callbacks.onResetAndProcess();
  } else {
    if (_callbacks.onProcessStep)
      _callbacks.onProcessStep();
  }

  if (isGenerationComplete())
    return;

  for (auto currValues : _waitingAgentsValues)
    _allAgentValues.push_back(currValues);
  _waitingAgentsValues.clear();

  for (std::size_t ii = 0; ii < _carsData.size(); ++ii)
    _carsData.at(ii).latestTransformsHistory.clear();

  for (std::size_t threadIndex = 0; threadIndex < _allThreadsData.size(); ++threadIndex)
  {
    auto taskCallback = [this, threadIndex, elapsedTime, totalSteps]() -> void {

      auto startTime = _getTime();

      //
      //

      auto& currData = *(_allThreadsData.at(threadIndex));

      auto& currPhysicWorlds = currData.physicWorld;

      for (unsigned int step = 0; step < totalSteps; ++step)
      {
        currData.frameProfiler.start();

        // update physical world

        constexpr uint32_t maxSubSteps = 0;
        constexpr float fixedTimeStep = 1.0f / 30.0f;

        currPhysicWorlds.step(fixedTimeStep, maxSubSteps, fixedTimeStep);

        // update cars

        for (auto currValues : _allAgentValues)
        {
          auto& currAgent = currValues->carAgent;

          if (!currAgent.isAlive())
            continue;

          if (!currAgent.isOwnedByPhysicWorld(&currPhysicWorlds))
            continue;

          currAgent.update(elapsedTime, currValues->neuralNet);

          {
            const auto body = currAgent.getBody();
            const auto vehicle = currAgent.getVehicle();

            CarData::CarTransform newData;

            // transformation matrix of the car
            newData.chassis.position = body->getPosition();
            newData.chassis.orientation = body->getOrientation();

            // transformation matrix of the wheels
            for (unsigned int jj = 0; jj < 4; ++jj)
            {
              auto& currWheel = newData.wheels.at(jj);

              currWheel.position = vehicle->getWheelPosition(jj);
              currWheel.orientation = vehicle->getWheelOrientation(jj);
            }

            _carsData.at(currValues->dataIndex).latestTransformsHistory.push_back(newData);
          }
        }

        currData.frameProfiler.stop(currPhysicWorlds.getPhysicVehicleManager().totalLiveVehicles());
      }

      //
      //

      auto& coreState = _coreStates.at(threadIndex);

      coreState.genomesAlive = _getTotalLiveAgents(currData);

      auto endTime = _getTime();
      auto milliSeconds = _asMilliSeconds(endTime - startTime);
      coreState.delta = milliSeconds.count();
    };

    _multithreadProducer.push(taskCallback);
  }

  // #if not defined D_NATIVE_PTHREAD_BUILD
  //     _multithreadProducer.waitUntilAllCompleted();
  // #endif
}

void
PthreadSimulation::breed() {

  if (!_multithreadProducer.allCompleted())
    return;
  if (!isGenerationComplete())
    return;

  // rate genomes
  for (std::size_t ii = 0; ii < _carsData.size(); ++ii) {
    // this penalty reward fast cars (reaching farther in less updates)
    const float fitnessPenalty = float(_carsData.at(ii).totalUpdates) / 10000;

    _geneticAlgorithm.rateGenome(ii, _carsData.at(ii).fitness - fitnessPenalty);
  }

  const bool isSmarter = _geneticAlgorithm.breedPopulation();

  if (_callbacks.onGenerationEnd)
    _callbacks.onGenerationEnd(isSmarter);

  //
  //

  _updateCarResult();

  _resetPhysic();

  _waitingAgentsValues.clear();
  _allAgentValues.clear();
  _currentAgentIndex = 0;
  _currentLiveAgents = 0;

  _addCars();

  // reset
  for (auto& currData : _carsData)
  {
    currData.totalUpdates = 0;
    currData.fitness = 0.0f;
  }

  _isFirstGenerationFrame = true;
}

bool
PthreadSimulation::isGenerationComplete() const {

  if (getWaitingGenomes() > 0)
    return false;

  for (const auto& currValue : _allAgentValues)
    if (currValue->carAgent.isAlive())
      return false;

  return true;
}

void
PthreadSimulation::_updateCarResult()
{
  for (const auto& currValue : _allAgentValues)
  {
    const auto& currAgent = currValue->carAgent;

    auto& carData = _carsData.at(currValue->dataIndex);

    const bool carWasAlive = carData.isAlive;
    carData.isAlive = currAgent.isAlive();
    carData.life = currAgent.getLife();
    carData.fitness = currAgent.getFitness();
    carData.totalUpdates = currAgent.getTotalUpdates();
    carData.groundIndex = currAgent.getGroundIndex();

    if (carWasAlive && !carData.isAlive) {

      _currentLiveAgents -= 1;
      _addCars();

      if (_callbacks.onGenomeDie)
        _callbacks.onGenomeDie(currValue->dataIndex);
    }

    if (!carData.isAlive) {

      // TODO: to check, and document if not a problem

      carData.latestTransformsHistory.clear();
      continue;
    }

    const auto body = currAgent.getBody();
    const auto vehicle = currAgent.getVehicle();

    // transformation matrix of the car
    carData.liveTransforms.chassis.position = body->getPosition();
    carData.liveTransforms.chassis.orientation = body->getOrientation();

    // transformation matrix of the wheels
    for (std::size_t jj = 0; jj < carData.liveTransforms.wheels.size(); ++jj)
    {
      carData.liveTransforms.wheels.at(jj).position = vehicle->getWheelPosition(jj);
      carData.liveTransforms.wheels.at(jj).orientation = vehicle->getWheelOrientation(jj);
    }

    carData.velocity = body->getLinearVelocity();

    const auto& eyeSensors = currAgent.getEyeSensors();
    for (std::size_t jj = 0; jj < eyeSensors.size(); ++jj) {

      const auto& inSensor = eyeSensors.at(jj);
      auto& outSensor = carData.eyeSensors.at(jj);

      outSensor.near = inSensor.near;
      outSensor.far = inSensor.far;
      outSensor.value = inSensor.value;
    }

    const auto& gSensor = currAgent.getGroundSensor();
    carData.groundSensor.near = gSensor.near;
    carData.groundSensor.far = gSensor.far;
    carData.groundSensor.value = gSensor.value;

    currValue->neuralNet.getNeuronsValues(carData.neuronsValues);
  }

  for (std::size_t index = 0; index < _allAgentValues.size(); )
  {
    auto& currValue = _allAgentValues.at(index);

    if (currValue->carAgent.isAlive() == false)
    {
      if (index + 1 < _allAgentValues.size())
        std::swap(currValue, _allAgentValues.back());
      _allAgentValues.pop_back();
    }
    else
    {
      ++index;
    }
  }

}

void PthreadSimulation::_addCars()
{
  while (_currentAgentIndex < _def.totalGenomes) {

    // select physic world with fewer cars

    std::size_t bestWorldIndex = 0;
    for (std::size_t ii = 1; ii < _allThreadsData.size(); ++ii)
    {
      const auto& bestState = _coreStates.at(bestWorldIndex);
      const auto& currState = _coreStates.at(ii);

      if (bestState.genomesAlive < currState.genomesAlive)
        continue;

      bestWorldIndex = ii;
    }

    auto& bestThreadData = *(_allThreadsData.at(bestWorldIndex));
    auto& physicWorld = bestThreadData.physicWorld;

    const uint32_t totalLiveVehicles = physicWorld.getPhysicVehicleManager().totalLiveVehicles();
    if (totalLiveVehicles > 20)
    {
      if (_waitingAgentsValues.size() >= 10)
        break;

      if (bestThreadData.frameProfiler.getMaxDelta(totalLiveVehicles) >= 30)
        break;
    }

    // make the car live

    auto newValue = std::make_shared<AgentValues>(
      _currentAgentIndex,
      _def.neuralNetworkTopology
    );
    newValue->carAgent.reset(
      &physicWorld,
      _startTransform.position,
      _startTransform.quaternion
    );
    _waitingAgentsValues.push_back(newValue);

    const auto& currGenome = _geneticAlgorithm.getGenome(_currentAgentIndex);

    newValue->neuralNet.setConnectionsWeights(currGenome.getConnectionsWeights());

    //

    _currentAgentIndex += 1;
    _currentLiveAgents += 1;

    for (std::size_t ii = 0; ii < _allThreadsData.size(); ++ii)
    {
      auto& coreState = _coreStates.at(ii);
      coreState.genomesAlive = _getTotalLiveAgents(*_allThreadsData.at(ii));
    }

  }

}

uint32_t PthreadSimulation::_getTotalLiveAgents(const ThreadData& inThreadData) const
{
  uint32_t totalLiveAgents = 0;

  const auto& currPhysicWorlds = inThreadData.physicWorld;

  for (auto& currValues : _waitingAgentsValues) {
    const auto& currAgent = currValues->carAgent;
    if (currAgent.isAlive() && currAgent.isOwnedByPhysicWorld(&currPhysicWorlds))
      totalLiveAgents++;
  }

  for (auto& currValues : _allAgentValues) {
    const auto& currAgent = currValues->carAgent;
    if (currAgent.isAlive() && currAgent.isOwnedByPhysicWorld(&currPhysicWorlds))
      totalLiveAgents++;
  }

  return totalLiveAgents;
}

unsigned int
PthreadSimulation::getTotalCores() const {
  return _def.totalCores;
}

const AbstractSimulation::CoreState&
PthreadSimulation::getCoreState(unsigned int index) const {
  return _coreStates.at(index);
}

const CarData&
PthreadSimulation::getCarResult(unsigned int index) const {
  return _carsData.at(index);
}

unsigned int
PthreadSimulation::getTotalCars() const {
  return _def.totalGenomes;
}

void
PthreadSimulation::setOnGenerationResetCallback(
  AbstractSimulation::SimpleCallback callback) {
  _callbacks.onResetAndProcess = callback;
}

void
PthreadSimulation::setOnGenerationStepCallback(
  AbstractSimulation::SimpleCallback callback) {
  _callbacks.onProcessStep = callback;
}

void
PthreadSimulation::setOnGenomeDieCallback(
  AbstractSimulation::GenomeDieCallback callback) {
  _callbacks.onGenomeDie = callback;
}

void
PthreadSimulation::setOnGenerationEndCallback(
  AbstractSimulation::GenerationEndCallback callback) {
  _callbacks.onGenerationEnd = callback;
}

std::size_t PthreadSimulation::getWaitingGenomes() const
{
  return _def.totalGenomes - _currentAgentIndex;
}

std::size_t PthreadSimulation::getLiveGenomes() const
{
  return std::size_t(_currentLiveAgents);
}

std::size_t PthreadSimulation::getTotalGenomes() const
{
  return _geneticAlgorithm.getTotalGenomes();
}
AbstractGenome& PthreadSimulation::getGenome(std::size_t inIndex)
{
  return _geneticAlgorithm.getGenome(inIndex);
}
const AbstractGenome& PthreadSimulation::getGenome(std::size_t inIndex) const
{
  return _geneticAlgorithm.getGenome(inIndex);
}

const AbstractGenome&
PthreadSimulation::getBestGenome() const {
  return _geneticAlgorithm.getBestGenome();
}

unsigned int
PthreadSimulation::getGenerationNumber() const {
  return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3&
PthreadSimulation::getStartPosition() const {
  return _startTransform.position;
}

const GeneticAlgorithm&
PthreadSimulation::getGeneticAlgorithm() const {
  return _geneticAlgorithm;
}
