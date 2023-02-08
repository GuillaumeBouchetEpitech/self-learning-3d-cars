
#pragma once

#include "application/context/Context.hpp"

#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"

class Scene {
public:
  static void initialise();
  static void renderSimple();
  static void renderAll();

  static void updateMatrices();
  static void renderScene(const gero::graphics::Camera& inCamera);

private:
  static void _clear();

private:
  static void _renderLeadingCarSensors();
  static void _renderHUD_ortho();
  static void _renderHUD();
};
