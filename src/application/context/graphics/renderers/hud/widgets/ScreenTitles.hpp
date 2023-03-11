
#pragma once

#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class ScreenTitles {
private:
  gero::Timer _alphaFadingTimer;

  gero::easing::GenericEasing<2> _backgroundEasing;
  gero::easing::GenericEasing<2> _mainTitleAlphaEasing;
  gero::easing::GenericEasing<2> _fitnessTitleAlphaEasing;
  gero::easing::GenericEasing<2> _commentTitleAlphaEasing;

  gero::Timer _titleAnimTimer;
  gero::easing::GenericEasing<2> _mainTitleAnimEasing;
  gero::easing::GenericEasing<2> _fitnessTitleAnimEasing;
  gero::easing::GenericEasing<2> _commentTitleAnimEasing;

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
