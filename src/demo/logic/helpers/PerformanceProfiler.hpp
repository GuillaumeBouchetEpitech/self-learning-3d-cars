
#pragma once

#include "geronimo/system/containers/fixed_size_array.hpp"

#include <chrono>
#include <string>
#include <unordered_map>

class PerformanceProfiler {
public:
  class TimeData {

    friend PerformanceProfiler;

  private:
    std::chrono::high_resolution_clock::time_point _start;
    std::chrono::high_resolution_clock::time_point _stop;
    int64_t _latestDuration = 0;
    gero::fixed_size_array<int64_t, int64_t, 60> _historicData;
    int32_t _historicIndex = 0;
    int64_t _averageDuration = 0;

  private:
    void _onStart();
    void _onStop();

  public:
    TimeData() = default;
    TimeData(const TimeData& other) = delete;
    TimeData(TimeData&& other) = delete;
    TimeData& operator=(const TimeData& other) = delete;
    TimeData& operator=(TimeData&& other) = delete;
    ~TimeData() = default;

  public:
    int64_t getLatestDuration() const;
    int64_t getAverageDuration() const;
  };

  using TimeDataMap = std::unordered_map<std::string, TimeData>;

private:
  TimeDataMap _allTimes;

public:
  PerformanceProfiler() = default;
  ~PerformanceProfiler() = default;

public:
  void start(const std::string& name);
  void stop(const std::string& name);
  const TimeDataMap& getTimeDataMap() const;
};
