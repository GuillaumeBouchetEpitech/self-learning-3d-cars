
#pragma once

#include "application/context/logic/simulation/logic/CarData.hpp"
#include "application/context/logic/simulation/logic/CircuitBuilder.hpp"

#include "basic-genetic-algorithm/Genome.hpp"
#include "basic-genetic-algorithm/NeuralNetworkTopology.hpp"

#include "geronimo/system/NonCopyable.hpp"

#include "application/defines.hpp"

#include <functional>
#include <memory>

class AbstractSimulation : public gero::NonCopyable {
public:
  using SimpleCallback = std::function<void()>;
  using GenerationEndCallback = std::function<void(bool)>;

  struct Definition {
    std::string filename;
    uint32_t totalGenomes = 0;
    uint32_t totalCores = 0;
    NeuralNetworkTopology neuralNetworkTopology;
    CircuitBuilder::CallbackNoNormals onSkeletonPatch = nullptr;
    CircuitBuilder::CallbackNormals onNewGroundPatch = nullptr;
    CircuitBuilder::CallbackNormals onNewLeftWallPatch = nullptr;
    CircuitBuilder::CallbackNormals onNewRightWallPatch = nullptr;

    Definition() = default;
  };

  struct CoreState {
    uint32_t delta = 0;
    uint32_t genomesAlive = 0;
  };

public:
  virtual ~AbstractSimulation() = default;

public:
  virtual void initialize(const Definition& def) = 0;

public:
  virtual void update(float elapsedTime, uint32_t totalSteps) = 0;
  virtual void breed() = 0;
  virtual bool isGenerationComplete() const = 0;

public:
  virtual uint32_t getTotalCores() const = 0;
  virtual const CoreState& getCoreState(uint32_t index) const = 0;
  virtual const CarData& getCarResult(uint32_t index) const = 0;
  virtual uint32_t getTotalCars() const = 0;

public:
#if defined D_WEB_WEBWORKER_BUILD
  virtual void setOnWorkersReadyCallback(SimpleCallback callback) = 0;
#endif

  virtual void setOnGenerationResetCallback(SimpleCallback callback) = 0;
  virtual void setOnGenerationStepCallback(SimpleCallback callback) = 0;
  virtual void setOnGenerationEndCallback(GenerationEndCallback callback) = 0;

public:
  virtual std::size_t getWaitingGenomes() const = 0;
  virtual std::size_t getLiveGenomes() const = 0;
  virtual std::size_t getTotalGenomes() const = 0;
  virtual AbstractGenome& getGenome(std::size_t inIndex) = 0;
  virtual const AbstractGenome& getGenome(std::size_t inIndex) const = 0;
  virtual const AbstractGenome& getBestGenome() const = 0;

public:
  virtual uint32_t getGenerationNumber() const = 0;
  virtual const glm::vec3& getStartPosition() const = 0;

public:
  static std::unique_ptr<AbstractSimulation> create();
};
