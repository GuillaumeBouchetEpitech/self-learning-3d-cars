
#pragma once

#include "application/context/simulation/logic/CarAgent.hpp"
#include "application/context/simulation/logic/CarData.hpp"
#include "application/context/simulation/logic/CircuitBuilder.hpp"
#include "application/context/simulation/utilities/FrameProfiler.hpp"
#include "application/context/simulation/webworker/common.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"

#include "application/defines.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

class WorkerConsumer : public gero::NonCopyable {

private:
  struct AgentValues {
    uint32_t dataIndex;
    CarAgent carAgent;
    NeuralNetwork neuralNet;
    std::vector<CarData::CarTransform> transformsHistory;

    AgentValues(uint32_t inDataIndex, const NeuralNetworkTopology& inNeuralNetworkTopology);
  };

private:
  std::unique_ptr<gero::physics::PhysicWorld> _physicWorld;

  std::vector<std::shared_ptr<AgentValues>> _allAgentValues;

  NeuralNetworkTopology _neuralNetworkTopology;

  CircuitBuilder::StartTransform _startTransform;
  CircuitBuilder _circuitBuilder;

  gero::messaging::MessageBuffer _messageToSend;

  FrameProfiler _frameProfiler;

public:
  WorkerConsumer() = default;

public:
  void processMessage(const char* dataPointer, int dataSize);

private:
  void _sendBackToProducer();

private:
  void _initializeSimulation(gero::messaging::MessageView& receivedMsg);
  void _addNewCars(gero::messaging::MessageView& receivedMsg);
  void _processSimulation(float elapsedTime, uint32_t totalSteps);

private:
  void _resetPhysic();
};
