
#pragma once

#include "application/context/logic/simulation/logic/CarAgent.hpp"
#include "application/context/logic/simulation/logic/CarData.hpp"
#include "application/context/logic/simulation/logic/CircuitBuilder.hpp"
// #include "application/context/logic/simulation/webworker/common.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/NonCopyable.hpp"
// #include "geronimo/system/messaging/MessageBuffer.hpp"
// #include "geronimo/system/messaging/MessageView.hpp"
#include "geronimo/system/metrics/HistoricalTimeData.hpp"

// #include "application/defines.hpp"

#include <memory>
// #include <unordered_map>
#include <vector>

class SimulationProcess : public gero::NonCopyable {
public:
  struct AgentValues {
    uint32_t dataIndex;
    CarAgent carAgent;
    NeuralNetwork neuralNet;
    std::vector<CarData::CarTransform> transformsHistory;

    AgentValues(uint32_t inDataIndex, const NeuralNetworkTopology& inNeuralNetworkTopology);
  };

public:
  SimulationProcess() = default;
  ~SimulationProcess() = default;

public:
  void initialize(
    const NeuralNetworkTopology& inNeuralNetworkTopology, const CircuitBuilder::StartTransform& inStartTransform,
    const CircuitBuilder::Knots& inCircuitKnots);

public:
  void reset();
  void process(float elapsedTime);
  void addNewCar(uint32_t dataIndex, const float* inWeightsData, std::size_t inWeightsLength);

  void cleanup();

public:
  const NeuralNetworkTopology& getNeuralNetworkTopology() const;

  const AgentValues& getAgentValuesByIndex(std::size_t index) const;
  std::size_t getTotalAgentValues() const;

  std::size_t getTotalAgentsAlive() const;

  gero::metrics::HistoricalTimeData& getHistoricalTimeData();
  const gero::metrics::HistoricalTimeData& getHistoricalTimeData() const;

private:
  std::unique_ptr<gero::physics::PhysicWorld> _physicWorld;

  std::vector<std::shared_ptr<AgentValues>> _allAgentValues;

  NeuralNetworkTopology _neuralNetworkTopology;

  CircuitBuilder::StartTransform _startTransform;
  CircuitBuilder _circuitBuilder;

  gero::metrics::HistoricalTimeData _historicalTimeData;
};
