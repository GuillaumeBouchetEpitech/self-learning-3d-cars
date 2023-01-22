
#pragma once

#include "demo/logic/simulation/logic/CarAgent.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/webworker/common.hpp"

#include "machine-learning/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"

#include "demo/defines.hpp"

#include <memory>
#include <vector>

class WorkerConsumer : public gero::NonCopyable {
private:
  unsigned int _genomesPerCore = 0;

  std::unique_ptr<gero::physics::PhysicWorld> _physicWorld;

  CarAgents _carAgents;

  std::vector<std::vector<CarData::Transforms>> _latestTransformsHistory;

  NeuralNetworkTopology _neuralNetworkTopology;
  std::vector<std::shared_ptr<NeuralNetwork>> _neuralNetworks;

  CircuitBuilder::StartTransform _startTransform;
  // CircuitBuilder::Knots _circuitKnots;
  CircuitBuilder _circuitBuilder;

  gero::messaging::MessageBuffer _messageToSend;

public:
  WorkerConsumer() = default;

public:
  void processMessage(const char* dataPointer, int dataSize);

private:
  void _sendBackToProducer();

private:
  void _initialiseSimulation(gero::messaging::MessageView& receivedMsg);
  void _resetSimulation(gero::messaging::MessageView& receivedMsg);
  void _processSimulation(float elapsedTime, unsigned int totalSteps);

private:
  void _resetPhysic();
};
