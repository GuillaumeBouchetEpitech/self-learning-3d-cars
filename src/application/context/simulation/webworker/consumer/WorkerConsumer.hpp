
#pragma once

#include "application/context/simulation/logic/SimulationProcess.hpp"

#include "application/context/simulation/webworker/common.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physics/PhysicWorld.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"
#include "geronimo/system/metrics/HistoricalTimeData.hpp"

#include "application/defines.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

class WorkerConsumer : public gero::NonCopyable {

private:
  CircuitBuilder::StartTransform _startTransform;

  gero::messaging::MessageBuffer _messageToSend;

  SimulationProcess _simulationProcess;

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
};
