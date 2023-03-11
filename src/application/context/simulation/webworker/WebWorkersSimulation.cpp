
#include "WebWorkersSimulation.hpp"

#include "application/defines.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

void
WebWorkersSimulation::initialize(const Definition& def) {
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

  CircuitBuilder circuit;
  circuit.parse(def.filename);

  circuit.generateWireFrameSkeleton(def.onSkeletonPatch);
  circuit.generateCircuitGeometry(def.onNewGroundPatch, def.onNewWallPatch);

  //

  _allCarsData.resize(def.totalGenomes);

  GeneticAlgorithm::Definition genAlgoDef;
  genAlgoDef.topology = def.neuralNetworkTopology;
  genAlgoDef.totalGenomes = def.totalGenomes;
  genAlgoDef.minimumMutations = 2;
  genAlgoDef.mutationMaxChance = 0.2f;
  genAlgoDef.mutationMaxEffect = 0.2f;

  gero::rng::RNG::ensureRandomSeed();

  genAlgoDef.getRandomCallback = []()
  {
    return gero::rng::RNG::getNormalisedValue();
  };

  _geneticAlgorithm.initialize(genAlgoDef);

  //

  _startPosition = circuit.getStartTransform().position;

  WorkerProducer::Definition workerDef;
  workerDef.startTransform = circuit.getStartTransform();
  workerDef.knots = circuit.getKnots(); // TODO: copy/reallocation
  workerDef.neuralNetworkTopology = def.neuralNetworkTopology;

  _workerProducers.reserve(def.totalCores);
  for (uint32_t coreIndex = 0; coreIndex < def.totalCores; ++coreIndex)
    _workerProducers.emplace_back(std::make_shared<WorkerProducer>(workerDef));

  _currentRequest = WorkerRequest::WorkersLoading;
}

void
WebWorkersSimulation::update(float elapsedTime, uint32_t totalSteps) {
  // do nothing if the worker(s) are:
  // => not initialized
  // => not finished working
  for (const auto& workerProducer : _workerProducers)
    if (!workerProducer->isLoaded() || workerProducer->isProcessing())
      return;

  // if this line is reached it mean the worker(s) are now available

  if (_currentRequest == WorkerRequest::WorkersLoading) {
    // ask to reset if any worker(s) are not yet updated
    // => it should only happen the first time

    if (_callbacks.onWorkersReady)
      _callbacks.onWorkersReady();

    _resetAndProcessSimulation(elapsedTime, totalSteps);
    return;
  }

  for (uint32_t index = 0; index < _def.totalGenomes; ++index) {

    const auto it = _agentsWorkerMap.find(index);
    if (it == _agentsWorkerMap.end())
      continue;

    const auto& latestData = it->second->getCarDataByDataIndex(index);

    auto& oldData = _allCarsData.at(index);

    if (oldData.isAlive && !latestData.isAlive)
    {
      _addNewAgents();

      if (_callbacks.onGenomeDie)
        _callbacks.onGenomeDie(index);
    }

    oldData = latestData; // TODO: copy/realloc (-_-)
  }
  _carsDataIsUpToDate = true;

  if (_currentRequest == WorkerRequest::ResetAndProcess) {
    if (_callbacks.onGenerationReset)
      _callbacks.onGenerationReset();
  } else if (_currentRequest == WorkerRequest::Process) {
    if (_callbacks.onGenerationStep)
      _callbacks.onGenerationStep();
  }

  if (isGenerationComplete())
    return;

  // ask the worker(s) to process/update the (physic) simulation
  _processSimulation(elapsedTime, totalSteps);
}

void
WebWorkersSimulation::breed() {
  if (!isGenerationComplete())
    return;

  for (uint32_t ii = 0; ii < _def.totalGenomes; ++ii) {
    const auto& carData = getCarResult(ii);

    // this penalty reward fast cars (reaching farther in less updates)
    const float fitnessPenalty = float(carData.totalUpdates) / 10000;

    _geneticAlgorithm.rateGenome(ii, carData.fitness - fitnessPenalty);
  }

  bool isSmarter = _geneticAlgorithm.breedPopulation();

  if (_callbacks.onGenerationEnd)
    _callbacks.onGenerationEnd(isSmarter);

  for (auto workerProducer : _workerProducers)
    workerProducer->cleanupDeadAgents();

  // reset
  for (auto& currData : _allCarsData)
  {
    currData.totalUpdates = 0;
    currData.fitness = 0.0f;
  }

  // ask the worker(s) to reset the (physic) simulation
  const float elapsedTime = 0.0f;
  const uint32_t totalSteps = 1;
  _resetAndProcessSimulation(elapsedTime, totalSteps);
}

bool
WebWorkersSimulation::isGenerationComplete() const {

  if (!_carsDataIsUpToDate)
    return false;

  for (uint32_t ii = 0; ii < _def.totalGenomes; ++ii)
    if (getCarResult(ii).isAlive)
      return false;

  return true;
}

void
WebWorkersSimulation::_processSimulation(
  float elapsedTime, uint32_t totalSteps) {
  for (auto workerProducer : _workerProducers)
    workerProducer->processSimulation(elapsedTime, totalSteps);

  _currentRequest = WorkerRequest::Process;
}

void
WebWorkersSimulation::_resetAndProcessSimulation(float elapsedTime, uint32_t totalSteps)
{
  _currentAgentIndex = 0;
  _agentsWorkerMap.clear();

  _addNewAgents();

  for (auto currProducer : _workerProducers)
    currProducer->resetAndProcessSimulation(elapsedTime, totalSteps);

  _carsDataIsUpToDate = false;
  _currentRequest = WorkerRequest::ResetAndProcess;
}

void WebWorkersSimulation::_addNewAgents()
{
  while (_currentAgentIndex < _def.totalGenomes)
  {
    // select the worker with fewest agents

    uint32_t bestWorkerIndex = 0;
    for (std::size_t index = 1; index < _workerProducers.size(); ++index)
    {
      const auto& bestWorker = _workerProducers.at(bestWorkerIndex);
      const auto& currWorker = _workerProducers.at(index);

      if (bestWorker->getTotalLiveAgents() < currWorker->getTotalLiveAgents())
        continue;

      bestWorkerIndex = index;
    }

    // make the car live

    const auto& targetWorker = _workerProducers.at(bestWorkerIndex);
    const auto& currGenome = _geneticAlgorithm.getGenome(_currentAgentIndex);
    if (targetWorker->addNewAgent(_currentAgentIndex, currGenome) == false)
      break;

    _agentsWorkerMap[_currentAgentIndex] = targetWorker;

    //

    _currentAgentIndex += 1;
  }
}

uint32_t
WebWorkersSimulation::getTotalCores() const {
  return _def.totalCores;
}

const AbstractSimulation::CoreState&
WebWorkersSimulation::getCoreState(uint32_t index) const {
  return _workerProducers.at(index)->getCoreState();
}

const CarData&
WebWorkersSimulation::getCarResult(uint32_t index) const {
  return _allCarsData.at(index);
}

uint32_t
WebWorkersSimulation::getTotalCars() const {
  return _def.totalGenomes;
}

void
WebWorkersSimulation::setOnWorkersReadyCallback(
  AbstractSimulation::SimpleCallback callback) {
  _callbacks.onWorkersReady = callback;
}

void
WebWorkersSimulation::setOnGenerationResetCallback(
  AbstractSimulation::SimpleCallback callback) {
  _callbacks.onGenerationReset = callback;
}

void
WebWorkersSimulation::setOnGenerationStepCallback(
  AbstractSimulation::SimpleCallback callback) {
  _callbacks.onGenerationStep = callback;
}

void
WebWorkersSimulation::setOnGenomeDieCallback(
  AbstractSimulation::GenomeDieCallback callback) {
  _callbacks.onGenomeDie = callback;
}

void
WebWorkersSimulation::setOnGenerationEndCallback(
  AbstractSimulation::GenerationEndCallback callback) {
  _callbacks.onGenerationEnd = callback;
}

std::size_t WebWorkersSimulation::getWaitingGenomes() const
{
  return std::size_t(_def.totalGenomes - _currentAgentIndex);
}

std::size_t WebWorkersSimulation::getLiveGenomes() const
{
  std::size_t totalLiveAgents = 0;
  for (const auto& worker : _workerProducers)
    totalLiveAgents += std::size_t(worker->getTotalLiveAgents());
  return totalLiveAgents;
}

std::size_t WebWorkersSimulation::getTotalGenomes() const
{
  return _geneticAlgorithm.getTotalGenomes();
}
AbstractGenome& WebWorkersSimulation::getGenome(std::size_t inIndex)
{
  return _geneticAlgorithm.getGenome(inIndex);
}
const AbstractGenome& WebWorkersSimulation::getGenome(std::size_t inIndex) const
{
  return _geneticAlgorithm.getGenome(inIndex);
}

const AbstractGenome&
WebWorkersSimulation::getBestGenome() const {
  return _geneticAlgorithm.getBestGenome();
}

uint32_t
WebWorkersSimulation::getGenerationNumber() const {
  return _geneticAlgorithm.getGenerationNumber();
}

const glm::vec3&
WebWorkersSimulation::getStartPosition() const {
  return _startPosition;
}

const GeneticAlgorithm&
WebWorkersSimulation::getGeneticAlgorithm() const {
  return _geneticAlgorithm;
}
