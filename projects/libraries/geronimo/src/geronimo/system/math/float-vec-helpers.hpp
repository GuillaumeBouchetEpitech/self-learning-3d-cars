
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace gero {
namespace math {

glm::vec2& safeNormalize(glm::vec2& direction);
glm::vec3& safeNormalize(glm::vec3& direction);
glm::vec2& limitate(glm::vec2& direction, float maxValue);
glm::vec3& limitate(glm::vec3& direction, float maxValue);

} // namespace math
} // namespace gero
