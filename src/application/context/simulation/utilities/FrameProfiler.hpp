
#pragma once

#include <unordered_map>
#include <cstdint>
#include <chrono>

class FrameProfiler
{
private:
  std::chrono::high_resolution_clock::time_point _startTime;

  std::unordered_map<uint32_t, int32_t> _deltasMap;

public:
  FrameProfiler() = default;
  FrameProfiler(const FrameProfiler& other) = delete;
  FrameProfiler(FrameProfiler&& other) = delete;
  FrameProfiler& operator=(const FrameProfiler& other) = delete;
  FrameProfiler& operator=(FrameProfiler&& other) = delete;
  ~FrameProfiler() = default;

public:
  void start();
  void stop(uint32_t totalLiveVehicles);
  void set(uint32_t totalLiveVehicles, int32_t deltaTime);

public:
  const std::unordered_map<uint32_t, int32_t>& getDeltasMap() const;

  int32_t getMaxDelta(uint32_t totalLiveAgents) const;

};
