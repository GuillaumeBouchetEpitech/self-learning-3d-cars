
#pragma once

#include "application/context/simulation/logic/CarAgent.hpp"
#include "application/context/simulation/logic/CarData.hpp"
#include "application/context/simulation/logic/CircuitBuilder.hpp"
#include "application/context/simulation/webworker/common.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"

#include "application/defines.hpp"

#include <memory>
#include <vector>

class WorkerConsumer : public gero::NonCopyable {
private:
  unsigned int _genomesPerCore = 0;

  std::unique_ptr<gero::physics::PhysicWorld> _physicWorld;

  CarAgents _carAgents;

  std::vector<std::vector<CarData::CarTransform>> _latestTransformsHistory;

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
