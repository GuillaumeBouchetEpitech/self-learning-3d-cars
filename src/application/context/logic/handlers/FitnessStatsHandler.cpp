
#include "FitnessStatsHandler.hpp"

#include "geronimo/system/TraceLogger.hpp"

FitnessStatsHandler::FitnessStatsHandler() {
  _index = 0;
  for (float& fitness : _allFitnesses)
    fitness = 0.0f;
}

void
FitnessStatsHandler::update(float latestFitness) {
  const float lastFitness = get(-1);

#if 1
  const float secondLastFitness = get(-2);
  const bool lastFitnessWasSmarter = lastFitness > secondLastFitness;
  if (lastFitnessWasSmarter)
    _index = (_index + 1) % _allFitnesses.size();
#else
  _index = (_index + 1) % _allFitnesses.size();
#endif

  _allFitnesses[int(_index) - 1] = latestFitness;
  _maxFitness = std::max(_maxFitness, latestFitness);
}

float
FitnessStatsHandler::get(int index) const {
  return _allFitnesses[int(_index) + index];
}

std::size_t
FitnessStatsHandler::size() const {
  return _allFitnesses.size();
}

float
FitnessStatsHandler::max() const {
  return _maxFitness;
}
