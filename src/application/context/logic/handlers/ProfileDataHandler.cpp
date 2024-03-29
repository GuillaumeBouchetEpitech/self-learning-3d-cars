
#include "ProfileDataHandler.hpp"

void
ProfileDataHandler::initialize(uint32_t totalCores, uint32_t maxStateHistory) {
  _maxStateHistory = maxStateHistory;
  _totalCores = totalCores;

  _latestStatesData.resize(totalCores);
  _statesHistory.resize(totalCores);
  for (auto& stateHistory : _statesHistory)
    stateHistory.resize(maxStateHistory);
}

void
ProfileDataHandler::clearLatest() {
  _latestStatesData.clear();
}

void
ProfileDataHandler::addToLatest(const AbstractSimulation::CoreState& latest) {
  _latestStatesData.push_back(latest);
}

void
ProfileDataHandler::pushLatest() {
  for (uint32_t ii = 0; ii < _latestStatesData.size(); ++ii)
    _statesHistory.at(ii).at(_currHistoryIndex) = _latestStatesData.at(ii);

  // rotate index
  _currHistoryIndex = (_currHistoryIndex + 1) % _maxStateHistory;
}

uint32_t
ProfileDataHandler::getMaxStateHistory() const {
  return _maxStateHistory;
}

uint32_t
ProfileDataHandler::getTotalCores() const {
  return _totalCores;
}

const AbstractSimulation::CoreState&
ProfileDataHandler::getCoreData(uint32_t index) const {
  return _latestStatesData.at(index);
}

const AbstractSimulation::CoreState&
ProfileDataHandler::getCoreHistoryData(uint32_t coreIndex, uint32_t dataIndex) const {

  const uint32_t actualIndex = (dataIndex + _currHistoryIndex) % _maxStateHistory;

  return _statesHistory.at(coreIndex).at(actualIndex);
}

uint32_t
ProfileDataHandler::getLatestTotalDelta() const {
  uint32_t totalDelta = 0;
  for (std::size_t ii = 0; ii < _latestStatesData.size(); ++ii)
    totalDelta += _latestStatesData.at(ii).delta;

  return totalDelta;
}

uint32_t
ProfileDataHandler::getAllTimeMaxDelta() const {
  uint32_t maxDelta = 0;

  for (uint32_t coreIndex = 0; coreIndex < _totalCores; ++coreIndex)
    for (uint32_t statIndex = 0; statIndex < _maxStateHistory; ++statIndex)
      maxDelta = std::max(maxDelta, _statesHistory.at(coreIndex).at(statIndex).delta);

  return maxDelta;
}
