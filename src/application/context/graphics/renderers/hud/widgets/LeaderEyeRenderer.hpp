
#pragma once

#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class LeaderEyeRenderer {
private:
  glm::vec2 _position = {0, 0};
  bool _isVisible = false;
  gero::Timer _timer;
  gero::easing::GenericEasing<2> _moveEasing;

public:
  void initialize();

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void render();
};
