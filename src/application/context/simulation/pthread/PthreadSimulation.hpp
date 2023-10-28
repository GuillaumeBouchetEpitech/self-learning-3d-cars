
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"

#include "application/context/simulation/logic/SimulationProcess.hpp"

#include "basic-genetic-algorithm/GeneticAlgorithm.hpp"
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
  gero::threading::Producer _multithreadProducer;

  Definition _def;

  GeneticAlgorithm _geneticAlgorithm;

  struct Callbacks {
    AbstractSimulation::SimpleCallback onResetAndProcess;
    AbstractSimulation::SimpleCallback onProcessStep;
    AbstractSimulation::GenerationEndCallback onGenerationEnd;
  } _callbacks;

  std::vector<std::unique_ptr<SimulationProcess>> _allProcesses;

  uint32_t _currentAgentIndex = 0;

  AllCarsData _carsData;

  CircuitBuilder::StartTransform _startTransform;
  std::vector<AbstractSimulation::CoreState> _coreStates;

  bool _isFirstGenerationFrame = true;

public:
  PthreadSimulation() = default;
  ~PthreadSimulation();

public:
  void initialize(const Definition& def) override;

public:
  void update(float elapsedTime, uint32_t totalSteps) override;
  void breed() override;
  bool isGenerationComplete() const override;

private:
  void _updateCarResult();
  void _addCars();

public:
  uint32_t getTotalCores() const override;
  const AbstractSimulation::CoreState& getCoreState(uint32_t inIndex) const override;
  const CarData& getCarResult(uint32_t inIndex) const override;
  uint32_t getTotalCars() const override;

public:
  void setOnGenerationResetCallback(AbstractSimulation::SimpleCallback callback) override;
  void setOnGenerationStepCallback(AbstractSimulation::SimpleCallback callback) override;
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
};
