
#include "PthreadSimulation.hpp"

#include "application/defines.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/aliasing.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

#include <chrono>

namespace {

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMilliSeconds, std::chrono::duration_cast<std::chrono::milliseconds>);

} // namespace

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
  genAlgoDef.minimumMutations = 1;
  genAlgoDef.mutationMaxChance = 0.2f;
  genAlgoDef.mutationMaxEffect = 0.2f;

  gero::rng::RNG::ensureRandomSeed();

  genAlgoDef.getRandomCallback = []() { return gero::rng::RNG::getNormalisedValue(); };

  _geneticAlgorithm.initialize(genAlgoDef);

// #if defined D_WEB_BUILD
//   // blocking on the main thread of the browser is not advised...
  constexpr bool k_avoidBlocking = true;
// #else
//   constexpr bool k_avoidBlocking = false;
// #endif

  _multithreadProducer.initialize(_def.totalCores, k_avoidBlocking);

  _coreStates.resize(_def.totalCores);

  CircuitBuilder tmpCircuitBuilder;
  tmpCircuitBuilder.parse(_def.filename);

  {
    auto onNewPhysicGroundPatch = [&](
                                    const CircuitBuilder::Vec3Array& vertices, const CircuitBuilder::Vec3Array& colors,
                                    const CircuitBuilder::Vec3Array& normals,
                                    const CircuitBuilder::Indices& indices) -> void {
      if (_def.onNewGroundPatch)
        _def.onNewGroundPatch(vertices, colors, normals, indices);
    };

    auto onNewLeftPhysicWallPatch = [&](
                                      const CircuitBuilder::Vec3Array& vertices,
                                      const CircuitBuilder::Vec3Array& colors, const CircuitBuilder::Vec3Array& normals,
                                      const CircuitBuilder::Indices& indices) -> void {
      if (_def.onNewLeftWallPatch)
        _def.onNewLeftWallPatch(vertices, colors, normals, indices);
    };
    auto onNewRightPhysicWallPatch =
      [&](
        const CircuitBuilder::Vec3Array& vertices, const CircuitBuilder::Vec3Array& colors,
        const CircuitBuilder::Vec3Array& normals, const CircuitBuilder::Indices& indices) -> void {
      if (_def.onNewRightWallPatch)
        _def.onNewRightWallPatch(vertices, colors, normals, indices);
    };

    tmpCircuitBuilder.generateWireFrameSkeleton(_def.onSkeletonPatch);
    tmpCircuitBuilder.generateCircuitGeometry(
      onNewPhysicGroundPatch, onNewLeftPhysicWallPatch, onNewRightPhysicWallPatch);
  }

  _startTransform = tmpCircuitBuilder.getStartTransform();

  _allProcesses.reserve(_def.totalCores);
  for (uint32_t ii = 0; ii < _def.totalCores; ++ii) {
    auto newProcess = std::make_unique<SimulationProcess>();
    newProcess->initialize(_def.neuralNetworkTopology, _startTransform, tmpCircuitBuilder.getKnots());
    newProcess->reset();
    newProcess->getHistoricalTimeData().setSize(20);

    _allProcesses.push_back(std::move(newProcess));
  }

  _carsData.resize(_def.totalGenomes);

  _addCars();
}

void
PthreadSimulation::update(float elapsedTime, uint32_t totalSteps) {

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

  _addCars();

  for (std::size_t ii = 0; ii < _carsData.size(); ++ii)
    _carsData.at(ii).latestTransformsHistory.clear();

  for (std::size_t threadIndex = 0; threadIndex < _allProcesses.size(); ++threadIndex) {
    auto taskCallback = [this, threadIndex, elapsedTime, totalSteps]() -> void {
      //
      //

      auto startTime = _getTime();

      //
      //

      auto& currProcess = *(_allProcesses.at(threadIndex));

      for (uint32_t step = 0; step < totalSteps; ++step) {
        currProcess.process(elapsedTime);
      }

      for (std::size_t ii = 0; ii < currProcess.getTotalAgentValues(); ++ii) {

        const auto& currAgent = currProcess.getAgentValuesByIndex(ii);

        if (!currAgent.carAgent.isAlive()) {
          continue;
        }

        auto& dataTransforms = _carsData.at(currAgent.dataIndex).latestTransformsHistory;

        for (const auto& currTransform : currAgent.transformsHistory) {
          dataTransforms.push_back(currTransform);
        }
      }

      //
      //

      auto endTime = _getTime();
      auto milliSeconds = _asMilliSeconds(endTime - startTime);

      //
      //

      auto& coreState = _coreStates.at(threadIndex);
      coreState.genomesAlive = currProcess.getTotalAgentsAlive();
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

  for (auto& currProcess : _allProcesses)
    currProcess->reset();

  // _waitingAgentsValues.clear();
  // _allAgentValues.clear();
  _currentAgentIndex = 0;

  _addCars();

  // reset
  for (auto& currData : _carsData) {
    currData.totalUpdates = 0;
    currData.fitness = 0.0f;
  }

  _isFirstGenerationFrame = true;
}

bool
PthreadSimulation::isGenerationComplete() const {

  if (getWaitingGenomes() > 0)
    return false;

  for (const auto& currValue : _allProcesses)
    if (currValue->getTotalAgentsAlive() > 0)
      return false;

  return true;
}

void
PthreadSimulation::_updateCarResult() {
  for (const auto& currProcess : _allProcesses) {

    for (std::size_t ii = 0; ii < currProcess->getTotalAgentValues(); ++ii) {
      const auto& currValue = currProcess->getAgentValuesByIndex(ii);
      auto& carData = _carsData.at(currValue.dataIndex);
      currValue.carAgent.getAsCarData(currValue.neuralNet, carData);
    }

    currProcess->cleanup();
  }
}

void
PthreadSimulation::_addCars() {

  for (auto& currProcess : _allProcesses) {

    auto& historicalTimeData = currProcess->getHistoricalTimeData();

    if (
      historicalTimeData.getTotalDurations() < historicalTimeData.getSize() ||
      historicalTimeData.getAverageDuration() > 8 || historicalTimeData.getMaxDuration() > 30) {
      continue;
    }

    // while (currProcess->getTotalAgentsAlive() < 90 && _currentAgentIndex < _def.totalGenomes)
    int32_t carsToAddLeft = 10;
    while (carsToAddLeft-- > 0 && _currentAgentIndex < _def.totalGenomes) {
      const auto& currGenome = _geneticAlgorithm.getGenome(_currentAgentIndex);

      const auto& weights = currGenome.getConnectionsWeights();

      currProcess->addNewCar(_currentAgentIndex, weights.data(), weights.size());

      _currentAgentIndex += 1;
    }

    historicalTimeData.reset();
  }

  for (std::size_t ii = 0; ii < _allProcesses.size(); ++ii) {
    const auto& currProcess = _allProcesses.at(ii);
    auto& coreState = _coreStates.at(ii);
    coreState.genomesAlive = currProcess->getTotalAgentsAlive();
  }
}

uint32_t
PthreadSimulation::getTotalCores() const {
  return _def.totalCores;
}

const AbstractSimulation::CoreState&
PthreadSimulation::getCoreState(uint32_t inIndex) const {
  return _coreStates.at(inIndex);
}

const CarData&
PthreadSimulation::getCarResult(uint32_t inIndex) const {
  return _carsData.at(inIndex);
}

uint32_t
PthreadSimulation::getTotalCars() const {
  return _def.totalGenomes;
}

void
PthreadSimulation::setOnGenerationResetCallback(AbstractSimulation::SimpleCallback callback) {
  _callbacks.onResetAndProcess = callback;
}

void
PthreadSimulation::setOnGenerationStepCallback(AbstractSimulation::SimpleCallback callback) {
  _callbacks.onProcessStep = callback;
}

void
PthreadSimulation::setOnGenerationEndCallback(AbstractSimulation::GenerationEndCallback callback) {
  _callbacks.onGenerationEnd = callback;
}

std::size_t
PthreadSimulation::getWaitingGenomes() const {
  return _def.totalGenomes - _currentAgentIndex;
}

std::size_t
PthreadSimulation::getLiveGenomes() const {
  std::size_t totalLiveAgents = 0;
  for (const auto& currProcess : _allProcesses)
    totalLiveAgents += currProcess->getTotalAgentsAlive();
  return totalLiveAgents;
}

std::size_t
PthreadSimulation::getTotalGenomes() const {
  return _geneticAlgorithm.getTotalGenomes();
}
AbstractGenome&
PthreadSimulation::getGenome(std::size_t inIndex) {
  return _geneticAlgorithm.getGenome(inIndex);
}
const AbstractGenome&
PthreadSimulation::getGenome(std::size_t inIndex) const {
  return _geneticAlgorithm.getGenome(inIndex);
}

const AbstractGenome&
PthreadSimulation::getBestGenome() const {
  return _geneticAlgorithm.getBestGenome();
}

uint32_t
PthreadSimulation::getGenerationNumber() const {
  return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3&
PthreadSimulation::getStartPosition() const {
  return _startTransform.position;
}
