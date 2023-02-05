
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace helpers {

  void renderTextBackground(
    float textDepth,
    const glm::vec4& colorA,
    const glm::vec4& colorB,
    float extraSizeA,
    float extraSizeB
  );

}
