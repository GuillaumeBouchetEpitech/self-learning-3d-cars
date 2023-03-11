
#pragma once

namespace Messages {
enum class FromProducer : int32_t {
  LoadWorker = 0,
  ResetAndProcessSimulation,
  ProcessSimulation,
  Unknown,
};

enum class FromConsumer : int32_t {
  WebWorkerLoaded = 0,
  SimulationResult,
  Unknown
};
}; // namespace Messages
