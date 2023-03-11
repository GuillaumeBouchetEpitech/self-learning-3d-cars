
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class CoreUsageRenderer {

private:
  glm::vec2 _position;
  glm::vec2 _size;

  bool _isVisible = false;
  gero::Timer _timer;
  gero::easing::GenericEasing<2> _moveEasing;

public:
  CoreUsageRenderer();

  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void renderWireFrame();
  void renderHudText();
};