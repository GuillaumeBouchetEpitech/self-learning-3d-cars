
#pragma once

#include "geronimo/graphics/advanced-concept/stackRenderers/StackRenderers.hpp"

class SceneStackRenderers : public gero::graphics::StackRenderers {
public:
  SceneStackRenderers() = default;
  ~SceneStackRenderers() = default;

public:
  void initialize();

};
