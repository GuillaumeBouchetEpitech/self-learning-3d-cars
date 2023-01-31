
#include "PerformanceProfiler.hpp"

#include "geronimo/system/aliasing.hpp"

#include <chrono>

namespace {

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMicroSeconds, std::chrono::duration_cast<std::chrono::microseconds>);

}

void
PerformanceProfiler::TimeData::_onStart()
{
  _start = _getTime();
}

void
PerformanceProfiler::TimeData::_onStop() {
  _stop = _getTime();

  if (_start > _stop)
    return;

  _latestDuration = _asMicroSeconds(_stop - _start).count();

  if (_historicData.size() == _historicData.capacity())
  {
    _historicIndex = (_historicIndex + 1) % _historicData.size();
    _historicData.at(_historicIndex) = _latestDuration;

    for (int64_t value : _historicData)
      _averageDuration += value;
    _averageDuration /= _historicData.size();
  }
  else
  {
    _historicData.push_back(_latestDuration);
  }
}

int64_t
PerformanceProfiler::TimeData::getLatestDuration() const {
  return _latestDuration;
}

int64_t
PerformanceProfiler::TimeData::getAverageDuration() const {
  return _averageDuration;
}

void
PerformanceProfiler::start(const std::string& name) {
  _allTimes[name]._onStart();
}

void
PerformanceProfiler::stop(const std::string& name) {
  _allTimes[name]._onStop();
}

const PerformanceProfiler::TimeDataMap&
PerformanceProfiler::getTimeDataMap() const {
  return _allTimes;
}
