
#pragma once

#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class ScreenTitles {
private:
  gero::Timer _timer;

  gero::easing::GenericEasing<3> _backgroundEasing;
  gero::easing::GenericEasing<3> _mainTitleEasing;
  gero::easing::GenericEasing<3> _fitnessTitleEasing;
  gero::easing::GenericEasing<3> _commentTitleEasing;

  float _backgroundAlpha = 1.0f;
  float _mainTitleAlpha = 0.0f;
  float _fitnessTitleAlpha = 0.0f;
  float _commentTitleAlpha = 0.0f;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);

  void update(float elapsedTime);
  void render();
};
