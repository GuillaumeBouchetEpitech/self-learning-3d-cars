
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"

#include <array>
#include <unordered_map>
#include <vector>

class AbstractSimulation;

class CarWheelsTrailsHandler : public gero::NonCopyable {
public:
  using WheelTrail = std::vector<glm::vec3>;

  struct WheelsTrail {
    std::array<WheelTrail, 4> wheels;
  };

private:
  std::unordered_map<uint64_t, uint32_t> _genomeIndexMap;

  std::vector<WheelsTrail> _allWheelsTrails;

public:
  CarWheelsTrailsHandler() = default;
  ~CarWheelsTrailsHandler() = default;

public:
  void reset(const AbstractSimulation& simulation);
  void push(uint32_t carIndex, uint32_t wheelIndex, const glm::vec3& value);

public:
  bool isEmpty() const;
  const WheelsTrail& getTrailByIndex(uint32_t index) const;
  const WheelsTrail& getTrailById(uint64_t id) const;
};
