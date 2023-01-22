
#pragma once

#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class InformationTextRenderer {
private:
  gero::Timer _timer;
  gero::easing::GenericEasing<2> _alphaEasing;
  float _alpha = 0.0f;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);

  void update(float elapsedTime);

  void render();
};
