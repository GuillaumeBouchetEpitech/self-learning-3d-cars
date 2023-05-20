
#include "CarWheelsTrails.hpp"

#include "application/context/simulation/AbstractSimulation.hpp"

#include "geronimo/system/ErrorHandler.hpp"

void
CarWheelsTrails::reset(const AbstractSimulation& simulation) {
  _genomeIndexMap.clear();

  const uint32_t totalCars = simulation.getTotalCars();

  if (_allWheelsTrails.size() < totalCars)
    _allWheelsTrails.resize(totalCars);

  for (auto& trail : _allWheelsTrails) {
    for (auto& wheel : trail.wheels) {
      wheel.reserve(2048); // pre-allocate
      wheel.clear();
    }
  }

  const std::size_t safeSize = std::min<std::size_t>(totalCars, simulation.getTotalGenomes());
  for (std::size_t ii = 0; ii < safeSize; ++ii) {
    // record the trail index with it's genome id in the lookup map
    _genomeIndexMap[simulation.getGenome(ii).getId()] = ii;
  }
}

void
CarWheelsTrails::push(uint32_t carIndex, uint32_t wheelIndex, const glm::vec3& value) {
  _allWheelsTrails.at(carIndex).wheels.at(wheelIndex).emplace_back(value);
}

bool
CarWheelsTrails::isEmpty() const {
  return _allWheelsTrails.empty();
}

const CarWheelsTrails::WheelsTrail&
CarWheelsTrails::getTrailByIndex(uint32_t index) const {
  return _allWheelsTrails.at(index);
}

const CarWheelsTrails::WheelsTrail&
CarWheelsTrails::getTrailById(uint64_t inId) const {
  auto it = _genomeIndexMap.find(inId);
  if (it == _genomeIndexMap.end())
    D_THROW(std::runtime_error, "trail not found, id: " << inId);

  return _allWheelsTrails.at(it->second);
}
