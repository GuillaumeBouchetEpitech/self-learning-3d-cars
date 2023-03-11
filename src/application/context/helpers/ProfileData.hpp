
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"

#include <vector>

class ProfileData {
private:
  using StatesData = std::vector<AbstractSimulation::CoreState>;
  using StatesHistory = std::vector<StatesData>;

  uint32_t _maxStateHistory = 60;
  uint32_t _totalCores = 0;
  uint32_t _currHistoryIndex = 0;
  StatesData _latestStatesData;
  StatesHistory _statesHistory;

public:
  ProfileData() = default;

public:
  void initialize(uint32_t totalCores, uint32_t maxStateHistory);

public:
  void clearLatest();
  void addToLatest(const AbstractSimulation::CoreState& latest);
  void pushLatest();

public:
  uint32_t getMaxStateHistory() const;
  uint32_t getTotalCores() const;

public:
  const AbstractSimulation::CoreState& getCoreData(uint32_t index) const;
  const AbstractSimulation::CoreState&
  getCoreHistoryData(uint32_t coreIndex, uint32_t dataIndex) const;

  uint32_t getLatestTotalDelta() const;
  uint32_t getAllTimeMaxDelta() const;
};
