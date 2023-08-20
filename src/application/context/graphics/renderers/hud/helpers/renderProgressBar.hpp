
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace helpers {

void renderProgressBar(
  const glm::vec2& inCenter, const glm::vec2& inSize, float inProgressValueA, float inProgressValueB, float inTextDepth,
  float inTextScale, const glm::vec4& inTextColor, const glm::vec4& inTextOutlineColor, float inBorderSize,
  const glm::vec4& inBorderColor, const glm::vec4& inBackgroundColor, const glm::vec4& inProgressColorA,
  const glm::vec4& inProgressColorB);

}
