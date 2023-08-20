
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"
#include "application/context/simulation/logic/CarAgent.hpp"
#include "application/context/simulation/logic/CircuitBuilder.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/metrics/HistoricalTimeData.hpp"
#include "geronimo/system/multithreading/Producer.hpp"

#include <array>
#include <list>
#include <memory>
#include <vector>

class PthreadSimulation : public AbstractSimulation {

private:
  struct AgentValues {
    uint64_t dataIndex;
    CarAgent carAgent;
    NeuralNetwork neuralNet;

    AgentValues(uint64_t inDataIndex, const NeuralNetworkTopology& inNeuralNetworkTopology);
  };

private:
  gero::threading::Producer _multithreadProducer;

  Definition _def;

  GeneticAlgorithm _geneticAlgorithm;

  struct Callbacks {
    AbstractSimulation::SimpleCallback onResetAndProcess;
    AbstractSimulation::SimpleCallback onProcessStep;
    AbstractSimulation::GenomeDieCallback onGenomeDie;
    AbstractSimulation::GenerationEndCallback onGenerationEnd;
  } _callbacks;

  struct ThreadData {
    /**
     * need multiple physic worlds so that we can divide the
     * physical update and raycasting inside different threads
     */
    gero::physics::PhysicWorld physicWorld;

    gero::metrics::HistoricalTimeData historicalTimeData;
  };
  std::vector<std::unique_ptr<ThreadData>> _allThreadsData;

  uint32_t _currentAgentIndex = 0;
  uint32_t _currentLiveAgents = 0;

  std::vector<std::shared_ptr<AgentValues>> _waitingAgentsValues;
  std::vector<std::shared_ptr<AgentValues>> _allAgentValues;

  AllCarsData _carsData;

  CircuitBuilder _circuitBuilder;
  CircuitBuilder::StartTransform _startTransform;
  std::vector<AbstractSimulation::CoreState> _coreStates;

  bool _isFirstGenerationFrame = true;

public:
  PthreadSimulation() = default;
  ~PthreadSimulation();

public:
  void initialize(const Definition& def) override;

private:
  void _resetPhysic();

public:
  void update(float elapsedTime, uint32_t totalSteps) override;
  void breed() override;
  bool isGenerationComplete() const override;

private:
  void _updateCarResult();
  void _addCars();
  uint32_t _getTotalLiveAgents(const ThreadData& inThreadData) const;

public:
  uint32_t getTotalCores() const override;
  const AbstractSimulation::CoreState& getCoreState(uint32_t inIndex) const override;
  const CarData& getCarResult(uint32_t inIndex) const override;
  uint32_t getTotalCars() const override;

public:
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
