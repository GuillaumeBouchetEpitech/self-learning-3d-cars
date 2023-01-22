
#include "float-vec-helpers.hpp"

namespace gero {
namespace math {

glm::vec2& safeNormalize(glm::vec2& direction) {
  const float magnitude = glm::length(direction);
  if (magnitude > 0.0f)
    direction = direction / magnitude;
  return direction;
}

glm::vec3& safeNormalize(glm::vec3& direction) {
  const float magnitude = glm::length(direction);
  if (magnitude > 0.0f)
    direction = direction / magnitude;
  return direction;
}

//
//
//

glm::vec2& limitate(glm::vec2& direction, float maxValue) {
  const float magnitude = glm::length(direction);
  if (magnitude > maxValue)
    direction = (direction / magnitude) * maxValue;
  return direction;
}

glm::vec3& limitate(glm::vec3& direction, float maxValue) {
  const float magnitude = glm::length(direction);
  if (magnitude > maxValue)
    direction = (direction / magnitude) * maxValue;
  return direction;
}

} // namespace math
} // namespace gero
