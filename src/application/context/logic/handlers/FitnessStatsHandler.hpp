
#pragma once

#include "geronimo/system/containers/static_array.hpp"
#include "geronimo/system/NonCopyable.hpp"

class FitnessStatsHandler : gero::NonCopyable {
private:
  std::size_t _index = 0;
  gero::static_array<float, 10> _allFitnesses;
  float _maxFitness = 0.0f;

public:
  FitnessStatsHandler();
  ~FitnessStatsHandler() = default;

public:
  void update(float latestFitness);

public:
  float get(int index) const;
  std::size_t size() const;
  float max() const;
};
