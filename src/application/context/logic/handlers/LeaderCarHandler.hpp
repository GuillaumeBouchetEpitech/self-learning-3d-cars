
#pragma once

#include "application/context/logic/simulation/logic/CarData.hpp"

#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/helpers/GLMath.hpp"

#include <optional>

// TODO: refactor -> use callbacks
class LeaderCarHandler : gero::NonCopyable {
private:
  float _countdownUntilNewLeader = 0;
  int _carIndex = -1;
  float _totalTimeAsLeader = 0;
  glm::vec3 _carPosition = {0, 0, 0};

public:
  LeaderCarHandler() = default;
  ~LeaderCarHandler() = default;

public:
  void update(float elapsedTime);
  void reset();

public:
  bool hasLeader() const;
  int leaderIndex() const;
  std::optional<glm::vec3> leaderPosition() const;
  std::optional<CarData> leaderData() const;
  float totalTimeAsLeader() const;
};
