
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace helpers {

void renderProgressBar(
  const glm::vec2& center, const glm::vec2& size, float progressValue,
  float textDepth, float textScale, const glm::vec4& textColor,
  const glm::vec4& textOutlineColor, float borderSize,
  const glm::vec4& borderColor, const glm::vec4& backgroundColor,
  const glm::vec4& progressColor);

}
