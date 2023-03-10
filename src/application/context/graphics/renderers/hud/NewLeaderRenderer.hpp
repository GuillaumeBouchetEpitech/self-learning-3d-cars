
#pragma once

#include "geronimo/helpers/GLMath.hpp"

struct NewLeaderRenderer {
private:
  bool _isVisible = false;
  glm::vec3 _screenCoord = {0, 0, 0};

public:
  void compute();
  void renderWireFrame();
  void renderHudText();
};
