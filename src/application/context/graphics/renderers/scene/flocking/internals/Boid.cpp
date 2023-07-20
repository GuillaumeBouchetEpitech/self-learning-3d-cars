
#include "Boid.hpp"

#include "geronimo/system/math/angles.hpp"
#include "geronimo/system/math/safe-normalize.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

Boid::Boid() {
  position = {-200, 0, 0};
  position.x += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);
  position.y += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);
  position.z += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);

  for (auto& item : trail) {
    item.position = position;
  }
}

void
Boid::seek(const glm::vec3& target, float coef) {
  glm::vec3 currentDirection = target - position;
  gero::math::safeNormalize(currentDirection);
  acceleration += currentDirection * coef;
}

void
Boid::flee(const glm::vec3& target, float coef) {
  return seek(target, -coef);
}

void
Boid::separate(const std::vector<Boid*>& boids, float coef) {
  glm::vec3 accumulatedDirections = {0, 0, 0};
  for (const Boid* other : boids) {
    if (this == other)
      continue;

    const glm::vec3 diff = position - other->position;

    // if (glm::length(diff) > radius)
    //   continue;

    accumulatedDirections += diff;
  }

  gero::math::safeNormalize(accumulatedDirections);

  acceleration += accumulatedDirections * coef;
}

void
Boid::strafe(const glm::vec3& target, float inHorizontalAngle, float inVerticalAngle, float coef) {
  glm::vec3 currentDirection = target - position;
  const float magnitude = gero::math::safeNormalize(currentDirection);
  if (magnitude == 0.0f)
    return;

  const float horizontalAngle = gero::math::getAngle(currentDirection.x, currentDirection.y);
  const float verticalAngle =
    gero::math::getAngle(glm::length(glm::vec2(currentDirection.x, currentDirection.y)), currentDirection.z);
  const float newHorizontalAngle = horizontalAngle + inHorizontalAngle;
  const float newVerticalAngle = verticalAngle + inVerticalAngle;

  const glm::vec2 horizontalDirection = gero::math::getDirection(newHorizontalAngle);
  const glm::vec3 newLeftAxis = glm::vec3(horizontalDirection, 0);

  const float upRadius = std::cos(newVerticalAngle);

  glm::vec3 newUpAxis;
  newUpAxis.z = std::sin(newVerticalAngle);
  newUpAxis.x = upRadius * std::cos(newHorizontalAngle);
  newUpAxis.y = upRadius * std::sin(newHorizontalAngle);

  acceleration += newLeftAxis * coef + newUpAxis * coef;
}

void
Boid::wander(float coef) {
  acceleration.x += gero::rng::RNG::getRangedValue(-coef, +coef);
  acceleration.y += gero::rng::RNG::getRangedValue(-coef, +coef);
  acceleration.z += gero::rng::RNG::getRangedValue(-coef, +coef);
}

void
Boid::updateTrail() {
  for (std::size_t ii = trail.size() - 1; ii > 0; --ii)
    trail.at(ii) = trail.at(ii - 1);
  trail.at(0).position = position;

  graphicData.trailData.clear();
  graphicData.trailData.reserve(trail.size());

  GraphicData::TrailData tmpData;

  for (std::size_t kk = 0; kk + 1 < trail.size(); ++kk) {
    const glm::vec3& posA = trail.at(kk + 0).position;
    const glm::vec3& posB = trail.at(kk + 1).position;

    const glm::vec3 rawDiff = posB - posA;
    const float magnitude = glm::length(rawDiff);
    if (magnitude == 0.0f)
      continue;

    const float horAngleRad = gero::math::getAngle(rawDiff.x, rawDiff.y);
    const float vertAngleRad = gero::math::getAngle(glm::length(glm::vec2(rawDiff.x, rawDiff.y)), -rawDiff.z);

    tmpData.length = magnitude * 2.0f;

    tmpData.orientation = glm::identity<glm::quat>();
    tmpData.orientation *= glm::angleAxis(horAngleRad, glm::vec3(0, 0, 1));
    tmpData.orientation *= glm::angleAxis(vertAngleRad, glm::vec3(0, 1, 0));

    tmpData.center = posA + rawDiff * 0.5f;

    graphicData.trailData.push_back(tmpData);
  }
}

void
Boid::applyAcceleration(float maxAcceleration, float maxVelocity, float elapsedTime) {
  // limitate acc
  const float accMagnitude = glm::length(acceleration);
  if (accMagnitude > maxAcceleration)
    acceleration = (acceleration / accMagnitude) * maxAcceleration;
  velocity += acceleration;

  // limitate vel
  const float velMagnitude = glm::length(velocity);
  if (velMagnitude > maxVelocity)
    velocity = (velocity / velMagnitude) * maxVelocity;
  position += velocity * 25.0f * elapsedTime;
}

bool
Boid::operator==(const Boid& other) const {
  return this == &other;
}
