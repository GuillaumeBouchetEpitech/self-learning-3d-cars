
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"
#include "application/context/simulation/logic/CarData.hpp"
#include "application/context/simulation/logic/CircuitBuilder.hpp"
#include "application/context/simulation/webworker/common.hpp"
#include "application/context/simulation/webworker/producer/WorkerProducer.hpp"

#include "basic-genetic-algorithm/GeneticAlgorithm.hpp"

#include "application/defines.hpp"

#include <array>
#include <list>
#include <memory>
#include <vector>

class WebWorkersSimulation : public AbstractSimulation {
private:
  enum class WorkerRequest : uint32_t {
    None = 0,
    WorkersLoading,
    ResetAndProcess,
    Process,
  } _currentRequest = WorkerRequest::None;

  glm::vec3 _startPosition;

  std::vector<std::shared_ptr<WorkerProducer>> _workerProducers;
  std::unordered_map<uint64_t, std::shared_ptr<WorkerProducer>> _agentsWorkerMap;

  uint32_t _currentAgentIndex = 0;

  AllCarsData _allCarsData;
  bool _carsDataIsUpToDate = false;

private:
  Definition _def;
  NeuralNetworkTopology _neuralNetworkTopology;
  GeneticAlgorithm _geneticAlgorithm;

  struct t_callbacks {
    AbstractSimulation::SimpleCallback onWorkersReady;
    AbstractSimulation::SimpleCallback onGenerationReset;
    AbstractSimulation::SimpleCallback onGenerationStep;
    AbstractSimulation::GenomeDieCallback onGenomeDie;
    AbstractSimulation::GenerationEndCallback onGenerationEnd;
  } _callbacks;

public:
  WebWorkersSimulation() = default;
  ~WebWorkersSimulation() = default;

public:
  void initialize(const Definition& def) override;

public:
  void update(float elapsedTime, uint32_t totalSteps) override;
  void breed() override;
  bool isGenerationComplete() const override;

private:
  void _processSimulation(float elapsedTime, uint32_t totalSteps);
  void _resetAndProcessSimulation(float elapsedTime, uint32_t totalSteps);
  void _addNewAgents();

public:
  uint32_t getTotalCores() const override;
  const AbstractSimulation::CoreState& getCoreState(uint32_t index) const override;
  const CarData& getCarResult(uint32_t index) const override;
  uint32_t getTotalCars() const override;

public:
  void setOnWorkersReadyCallback(AbstractSimulation::SimpleCallback callback) override;
  void setOnGenerationResetCallback(AbstractSimulation::SimpleCallback callback) override;
  void setOnGenerationStepCallback(AbstractSimulation::SimpleCallback callback) override;
  void setOnGenomeDieCallback(AbstractSimulation::GenomeDieCallback callback) override;
  void setOnGenerationEndCallback(AbstractSimulation::GenerationEndCallback callback) override;

public:
  std::size_t getWaitingGenomes() const override;
  std::size_t getLiveGenomes() const override;
  std::size_t getTotalGenomes() const override;
  AbstractGenome& getGenome(std::size_t inIndex) override;
  const AbstractGenome& getGenome(std::size_t inIndex) const override;
  const AbstractGenome& getBestGenome() const override;

public:
  uint32_t getGenerationNumber() const override;
  const glm::vec3& getStartPosition() const override;
  const GeneticAlgorithm& getGeneticAlgorithm() const override;
};
