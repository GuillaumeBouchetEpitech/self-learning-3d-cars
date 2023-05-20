
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"
#include "application/context/simulation/logic/CarData.hpp"
#include "application/context/simulation/logic/CircuitBuilder.hpp"
#include "application/context/simulation/utilities/FrameProfiler.hpp"
#include "application/context/simulation/webworker/common.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"

#include "application/defines.hpp"

#include <bitset>

#include <emscripten/emscripten.h> // <= worker_handle

class WorkerProducer : public gero::NonCopyable {
public:
  struct Definition {
    CircuitBuilder::StartTransform startTransform;
    CircuitBuilder::Knots knots;
    NeuralNetworkTopology neuralNetworkTopology;

    Definition() = default;
  };

private:
  worker_handle _workerHandle;

  Definition _def;

  uint32_t _currentLiveAgents = 0;

  enum class Status : unsigned int { WebWorkerLoaded = 0, Processing, Updated, Count };

  std::bitset<gero::asValue(Status::Count)> _flags;

  AbstractSimulation::CoreState _coreState;

  struct AgentData {
    uint32_t dataIndex = 0;
    CarData carData;

    std::vector<float> connectionsWeights;

    AgentData(uint32_t inDataIndex);
  };

  std::vector<std::shared_ptr<AgentData>> _waitingAgentsData;
  std::vector<std::shared_ptr<AgentData>> _allAgentsData;
  std::unordered_map<uint32_t, std::shared_ptr<AgentData>> _agentsDataMap;

  FrameProfiler _frameProfiler;

  gero::messaging::MessageBuffer _message;

public:
  WorkerProducer(const Definition& def);

private:
  static void _onMessageCallback(char* dataPointer, int dataSize, void* arg);

private:
  void _processMessage(const char* pData, int dataLength);

private:
  void _sendToConsumer();

public:
  void resetAndProcessSimulation(float elapsedTime, unsigned int totalSteps);
  void processSimulation(float elapsedTime, unsigned int totalSteps);

private:
  void _fillMessageWithAgentToAdd();

public:
  bool addNewAgent(uint32_t inDataIndex, const AbstractGenome& inGenome);

public:
  void cleanupDeadAgents();

public:
  bool isLoaded() const;
  bool isProcessing() const;
  bool isUpdated() const;

  const CarData& getCarDataByDataIndex(uint32_t inDataIndex) const;
  const CarData& getCarDataByIndex(uint32_t inIndex) const;
  std::size_t getTotalCarsData() const;

  const AbstractSimulation::CoreState& getCoreState() const;

  uint32_t getTotalLiveAgents() const;
};
