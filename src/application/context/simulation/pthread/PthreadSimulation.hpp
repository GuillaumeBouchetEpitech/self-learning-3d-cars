
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"
#include "application/context/simulation/logic/CarAgent.hpp"
#include "application/context/simulation/logic/CircuitBuilder.hpp"
#include "application/context/simulation/utilities/FrameProfiler.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/multithreading/Producer.hpp"
// #include "geronimo/system/containers/weak_ref_data_pool.hpp"

#include <array>
#include <list>
#include <memory>
#include <vector>

class PthreadSimulation : public AbstractSimulation {


private:
  struct AgentValues
  {
    uint64_t dataIndex;
    CarAgent carAgent;
    NeuralNetwork neuralNet;

    AgentValues(
      uint64_t inDataIndex,
      const NeuralNetworkTopology& inNeuralNetworkTopology
    );
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


  struct ThreadData
  {
    /**
     * need multiple physic worlds so that we can divide the
     * physical update and raycasting inside different threads
     */
    gero::physics::PhysicWorld physicWorld;

    FrameProfiler frameProfiler;
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
  virtual ~PthreadSimulation();

public:
  virtual void initialise(const Definition& def) override;

private:
  void _resetPhysic();

public:
  virtual void update(float elapsedTime, uint32_t totalSteps) override;
  virtual void breed() override;
  virtual bool isGenerationComplete() const override;

private:
  void _updateCarResult();
  void _addCars();
  uint32_t _getTotalLiveAgents(const ThreadData& inThreadData) const;

public:
  virtual uint32_t getTotalCores() const override;
  virtual const AbstractSimulation::CoreState&
  getCoreState(uint32_t index) const override;
  virtual const CarData& getCarResult(uint32_t index) const override;
  virtual uint32_t getTotalCars() const override;

public:
  virtual void setOnGenerationResetCallback(
    AbstractSimulation::SimpleCallback callback) override;
  virtual void setOnGenerationStepCallback(
    AbstractSimulation::SimpleCallback callback) override;
  virtual void setOnGenomeDieCallback(
    AbstractSimulation::GenomeDieCallback callback) override;
  virtual void setOnGenerationEndCallback(
    AbstractSimulation::GenerationEndCallback callback) override;

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
