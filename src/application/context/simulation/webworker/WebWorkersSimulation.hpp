
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
  virtual ~WebWorkersSimulation() = default;

public:
  virtual void initialize(const Definition& def) override;

public:
  virtual void update(float elapsedTime, uint32_t totalSteps) override;
  virtual void breed() override;
  virtual bool isGenerationComplete() const override;

private:
  void _processSimulation(float elapsedTime, uint32_t totalSteps);
  void _resetAndProcessSimulation(float elapsedTime, uint32_t totalSteps);
  void _addNewAgents();

public:
  virtual uint32_t getTotalCores() const override;
  virtual const AbstractSimulation::CoreState& getCoreState(uint32_t index) const override;
  virtual const CarData& getCarResult(uint32_t index) const override;
  virtual uint32_t getTotalCars() const override;

public:
  virtual void setOnWorkersReadyCallback(AbstractSimulation::SimpleCallback callback) override;
  virtual void setOnGenerationResetCallback(AbstractSimulation::SimpleCallback callback) override;
  virtual void setOnGenerationStepCallback(AbstractSimulation::SimpleCallback callback) override;
  virtual void setOnGenomeDieCallback(AbstractSimulation::GenomeDieCallback callback) override;
  virtual void setOnGenerationEndCallback(AbstractSimulation::GenerationEndCallback callback) override;

public:
  virtual std::size_t getWaitingGenomes() const override;
  virtual std::size_t getLiveGenomes() const override;
  virtual std::size_t getTotalGenomes() const override;
  virtual AbstractGenome& getGenome(std::size_t inIndex) override;
  virtual const AbstractGenome& getGenome(std::size_t inIndex) const override;
  virtual const AbstractGenome& getBestGenome() const override;

public:
  virtual uint32_t getGenerationNumber() const override;
  virtual const glm::vec3& getStartPosition() const override;
  virtual const GeneticAlgorithm& getGeneticAlgorithm() const override;
};
