
#pragma once

#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"

struct NewLeaderRenderer {
private:
  bool _isVisible = false;
  glm::vec3 _screenCoord = {0, 0, 0};

public:
  void compute(const gero::graphics::ICamera& cameraInstance);
  void renderWireFrame();
  void renderHudText();
};
