
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace gero {
namespace math {

struct Circle {
  glm::vec2 center;
  float radius;
};

bool circleCircleIntersectionPoints(const Circle& c1, const Circle& c2,
                                    glm::vec2& res1, glm::vec2& res2);

} // namespace math
} // namespace gero
