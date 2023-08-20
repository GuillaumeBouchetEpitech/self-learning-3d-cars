
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <unordered_set>

class MouseManager {
private:
  MouseManager() = default;
  ~MouseManager() = default;

private:
  static MouseManager* _instance;

public:
  static void create();
  static void destroy();
  static MouseManager& get();

public:
  void updateAsPressed(int inKey);
  void updateAsReleased(int inKey);
  void updateDelta(float x, float y);

public:
  bool isPressed(int inKey) const;

  template <typename... Args> bool isPressed(Args... args) const {
    std::array<int, sizeof...(args)> allValues{{args...}};

    for (int val : allValues)
      if (isPressed(val))
        return true;
    return false;
  }

public:
  const glm::vec2& getDelta() const;
  void resetDelta();

public:
  void reset();

public:
  bool isLocked() const;
  bool setLock(bool isLocked);

private:
  std::unordered_set<int> _pressedKeys;
  glm::vec2 _delta = {0, 0};
  bool _isLocked = false;
};
