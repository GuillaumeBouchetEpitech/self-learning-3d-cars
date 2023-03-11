
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class FitnessDataRenderer {

private:
  // struct Layout
  // {
  // }
  // _layout;

  glm::vec2 _position;
  glm::vec2 _size;
  float _alpha = 0.0f;

  bool _isVisible = false;
  gero::Timer _timer;
  gero::easing::GenericEasing<2> _moveEasing;
  gero::easing::GenericEasing<2> _alphaEasing;

public:
  FitnessDataRenderer();

  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void renderWireFrame();
  void renderHudText();
};