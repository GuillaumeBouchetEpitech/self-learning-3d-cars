
#include "FrameProfiler.hpp"

#include "geronimo/system/aliasing.hpp"

#include <chrono>

namespace {

D_ALIAS_FUNCTION(_getTime, std::chrono::high_resolution_clock::now);
D_ALIAS_FUNCTION(_asMilliSeconds, std::chrono::duration_cast<std::chrono::milliseconds>);

} // namespace

void
FrameProfiler::start() {
  _startTime = _getTime();
}

void
FrameProfiler::stop(uint32_t totalLiveVehicles) {
  auto stopTime = _getTime();
  const int32_t stepMilliSeconds = _asMilliSeconds(stopTime - _startTime).count();
  set(totalLiveVehicles, stepMilliSeconds);
}

void
FrameProfiler::set(uint32_t totalLiveVehicles, int32_t deltaTime) {
  auto it = _deltasMap.find(totalLiveVehicles);
  if (it == _deltasMap.end()) {
    // add
    _deltasMap[totalLiveVehicles] = deltaTime;
  } else {
    // update
    it->second = deltaTime;
  }
}

//
//
//

const std::unordered_map<uint32_t, int32_t>&
FrameProfiler::getDeltasMap() const {
  return _deltasMap;
}

int32_t
FrameProfiler::getMaxDelta(uint32_t totalLiveAgents) const {
  auto currIt = _deltasMap.find(totalLiveAgents);
  auto nextIt = _deltasMap.find(totalLiveAgents + 1);

  const uint32_t currDelta = currIt != _deltasMap.end() ? currIt->second : 0;
  const uint32_t nextDelta = nextIt != _deltasMap.end() ? nextIt->second : 0;

  return std::max(currDelta, nextDelta);
}
