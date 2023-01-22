
#pragma once

#include "demo/logic/simulation/AbstactSimulation.hpp"
#include "demo/logic/simulation/logic/CarData.hpp"
#include "demo/logic/simulation/logic/CircuitBuilder.hpp"
#include "demo/logic/simulation/webworker/common.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"

#include "demo/defines.hpp"

#include <bitset>

#include <emscripten/emscripten.h> // <= worker_handle

class WorkerProducer : public gero::NonCopyable {
public:
  struct Definition {
    CircuitBuilder::StartTransform startTransform;
    CircuitBuilder::Knots knots;
    unsigned int genomesPerCore = 0;
    NeuralNetworkTopology neuralNetworkTopology;

    Definition() = default;
  };

private:
  worker_handle _workerHandle;

  Definition _def;
  GeneticAlgorithm& _geneticAlgorithm;
  uint32_t _coreIndex;

  enum class Status : unsigned int {
    WebWorkerLoaded = 0,
    Processing,
    Updated,
    Count
  };

  std::bitset<gero::asValue(Status::Count)> _flags;

  AbstactSimulation::CoreState _coreState;

  CarDatas _carsData;

  gero::messaging::MessageBuffer _message;

public:
  WorkerProducer(
    const Definition& def, GeneticAlgorithm& geneticAlgorithm,
    uint32_t coreIndex);

private:
  static void _onMessageCallback(char* dataPointer, int dataSize, void* arg);

private:
  void _processMessage(const char* pData, int dataLength);

private:
  void _sendToConsumer();

public:
  void resetAndProcessSimulation(
    float elapsedTime, unsigned int totalSteps,
    const NeuralNetworks& neuralNetworks);
  void processSimulation(float elapsedTime, unsigned int totalSteps);

public:
  bool isLoaded() const;
  bool isProcessing() const;
  bool isUpdated() const;
  const CarDatas& getCarsData() const;

  const AbstactSimulation::CoreState& getCoreState() const;
};
