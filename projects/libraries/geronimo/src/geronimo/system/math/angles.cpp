
#include "angles.hpp"

#include "constants.hpp"

#include <cmath>

namespace gero {
namespace math {

float getAngle(const glm::vec2& direction) {
  return std::atan2(direction.y, direction.x);
}

glm::vec2 getDirection(float angle) {
  return glm::vec2(std::cos(angle), std::sin(angle));
}

glm::vec2 rotate2d(const glm::vec2& point, const glm::vec2& center,
                   float angle) {
  const glm::vec2 dir = getDirection(angle);
  const glm::vec2 diff = point - center;

  return glm::vec2(center.x + diff.x * dir.x + diff.y * dir.y,
                   center.y + diff.x * dir.y - diff.y * dir.x);
}

glm::vec3 rotate2d(const glm::vec3& point, const glm::vec2& center,
                   float angle) {
  return glm::vec3(rotate2d(glm::vec2(point), center, angle), point.z);
}

float getDesiredAngleFromAngle(float inCurrentRadAngle, float inDesiredRadAngle,
                               float inRadRotationSpeed) {

  //
  //
  // get delta angle

  float deltaRadAngle = inDesiredRadAngle - inCurrentRadAngle;

  if (deltaRadAngle > +math::pi)
    deltaRadAngle -= math::pi2;
  if (deltaRadAngle < -math::pi)
    deltaRadAngle += math::pi2;

  deltaRadAngle =
    (deltaRadAngle > 0
       ? (deltaRadAngle < +inRadRotationSpeed ? deltaRadAngle
                                              : +inRadRotationSpeed)
       : (deltaRadAngle > -inRadRotationSpeed ? deltaRadAngle
                                              : -inRadRotationSpeed));

  //
  //
  // determine final angle

  float finalRadAngle = inCurrentRadAngle + deltaRadAngle;

  if (finalRadAngle > math::pi2)
    finalRadAngle -= math::pi2;
  if (finalRadAngle < 0)
    finalRadAngle += math::pi2;

  return finalRadAngle;
}

float getDesiredAngleFromDestination(const glm::vec2& position,
                                     const glm::vec2& target,
                                     float currentAngle,
                                     float inRadRotationSpeed) {
  const float desiredAngle = getAngle(target - position);

  return getDesiredAngleFromAngle(desiredAngle, currentAngle,
                                  inRadRotationSpeed);
}

} // namespace math
} // namespace gero
