
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <unordered_map>
#include <vector>

class AbstractSimulation;

class CarWheelsTrails {
public:
  using WheelTrail = std::vector<glm::vec3>;

  struct WheelsTrail {
    std::array<WheelTrail, 4> wheels;
  };

private:
  std::unordered_map<uint64_t, uint32_t> _genomeIndexMap;

  std::vector<WheelsTrail> _allWheelsTrails;

public:
  CarWheelsTrails() = default;
  CarWheelsTrails(const CarWheelsTrails& other) = delete;
  CarWheelsTrails(CarWheelsTrails&& other) = delete;
  CarWheelsTrails& operator=(const CarWheelsTrails& other) = delete;
  CarWheelsTrails& operator=(CarWheelsTrails&& other) = delete;
  ~CarWheelsTrails() = default;

public:
  void reset(const AbstractSimulation& simulation);
  void push(uint32_t carIndex, uint32_t wheelIndex, const glm::vec3& value);

public:
  bool isEmpty() const;
  const WheelsTrail& getTrailByIndex(uint32_t index) const;
  const WheelsTrail& getTrailById(uint64_t id) const;
};
