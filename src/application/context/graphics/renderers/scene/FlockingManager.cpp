
#include "FlockingManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/make-geometries/MakeGeometries.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/math/lerp.hpp"
#include "geronimo/system/math/angles.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/math/safe-normalize.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

FlockingManager::Boid::Boid() {
  position = {-200, 0, 0};
  position.x += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);
  position.y += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);
  position.z += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);

  for (auto& item : trail)
  {
    item.position = position;
  }
}

void
FlockingManager::Boid::seek(const glm::vec3& target, float coef) {
  glm::vec3 currentDirection = target - position;
  gero::math::safeNormalize(currentDirection);
  acceleration += currentDirection * coef;
}

void
FlockingManager::Boid::flee(const glm::vec3& target, float coef) {
  return seek(target, -coef);
}

void
FlockingManager::Boid::separate(const std::vector<Boid>& boids, float radius, float coef) {
  glm::vec3 accumulatedDirections = {0, 0, 0};
  for (const Boid& other : boids) {
    if (*this == other)
      continue;

    const glm::vec3 diff = position - other.position;

    if (glm::length(diff) > radius)
      continue;

    accumulatedDirections += diff;
  }

  gero::math::safeNormalize(accumulatedDirections);

  acceleration += accumulatedDirections * coef;
}

void
FlockingManager::Boid::strafe(const glm::vec3& target, float inHorizontalAngle, float inVerticalAngle, float coef) {
  glm::vec3 currentDirection = target - position;
  const float magnitude = gero::math::safeNormalize(currentDirection);
  if (magnitude == 0.0f)
    return;

  const float horizontalAngle = gero::math::getAngle(currentDirection.x, currentDirection.y);
  const float verticalAngle = gero::math::getAngle(glm::length(glm::vec2(currentDirection.x, currentDirection.y)), currentDirection.z);
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
FlockingManager::Boid::wander(float coef) {
  acceleration.x += gero::rng::RNG::getRangedValue(-coef, +coef);
  acceleration.y += gero::rng::RNG::getRangedValue(-coef, +coef);
  acceleration.z += gero::rng::RNG::getRangedValue(-coef, +coef);
}

void FlockingManager::Boid::updateTrail()
{
  for (std::size_t ii = trail.size() - 1; ii > 0; --ii)
    trail.at(ii) = trail.at(ii - 1);
  trail.at(0).position = position;
}

void
FlockingManager::Boid::applyAcceleration(float maxAcceleration, float maxVelocity, float elapsedTime) {
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
FlockingManager::Boid::operator==(const Boid& other) const {
  return this == &other;
}

FlockingManager::FlockingManager() {
  constexpr std::size_t totalEntities = 16;
  _boids.resize(totalEntities);

  for (std::size_t ii = 0; ii < _boids.size(); ++ii)
  {
    auto& currBoid = _boids.at(ii);
    currBoid.horizontalAngle = gero::math::hpi * (((ii % 2) == 0) ? 1.0f : -1.0f);
    currBoid.verticalAngle = gero::math::hpi * (((ii % 3) == 0) ? 1.0f : -1.0f);
  }

}

void
FlockingManager::update(float elapsedTime) {
  auto& context = Context::get();
  const auto& logic = context.logic;
  const auto& simulation = *logic.simulation;
  glm::vec3 target = simulation.getStartPosition();
  const auto& leaderCar = logic.leaderCar;

  // valid leading car?
  if (auto leaderData = leaderCar.leaderData()) {
    // leading car alive?
    if (leaderData->isAlive) {
      target = leaderData->liveTransforms.chassis.position;
    }
  }

  constexpr float k_timePerTrailUpdate = 1.0f / 60.0f;

  bool needTrailUpdate = false;
  if (_timeUntilTrailUpdate > 0.0f)
    _timeUntilTrailUpdate -= elapsedTime;

  if (_timeUntilTrailUpdate <= 0.0f) {
    _timeUntilTrailUpdate = k_timePerTrailUpdate;
    needTrailUpdate = true;
  }

  constexpr float maxAcc = 0.05f;
  constexpr float maxVel = 2.0f;

  for (Boid& currBoid : _boids) {

    currBoid.acceleration = {0, 0, 0};

    const float distanceToTarget = glm::distance(currBoid.position, target);

    if (distanceToTarget > 30.0f)
    {
      currBoid.seek(target, 1.0f);
    }
    else if (distanceToTarget < 10.0f)
    {
      currBoid.flee(target, 3.0f);
    }
    else
    {
      currBoid.strafe(target, currBoid.horizontalAngle, currBoid.verticalAngle, 1.5f);
    }

    currBoid.separate(_boids, 1.5f, 2.0f);
    currBoid.wander(0.5f);

    constexpr float k_maxDistance = 60.0f;
    const float coef = 1.0f + 1.0f * distanceToTarget / k_maxDistance;
    currBoid.applyAcceleration(maxAcc * coef, maxVel * coef, elapsedTime);

    if (needTrailUpdate) {
      currBoid.updateTrail();
    }
  }
}

void
FlockingManager::render() {

  if (_boids.empty())
    return;

  auto& scene = Context::get().graphic.scene;

  {

    GeometriesStackRenderer::GeometryInstance instance;
    instance.position = glm::vec3(0.0f), instance.orientation = glm::quat(0.0f, glm::vec3(0, 0, 1));
    instance.scale = glm::vec3(0.4f);
    instance.color = glm::vec4(0.6f, 0.6f, 0.0f, 1.0f);
    instance.outlineValue = 1.0f;

    for (Boid& boid : _boids) {
      instance.position = boid.position;
      scene.geometriesStackRenderer.pushAlias(1111, instance);
    }
  }

  {
    auto& stackRenderer = scene.stackRenderers.getTrianglesStack();

    const glm::vec4 k_color = glm::vec4(0.8f, 0.8f, 0.8f, 0.4f);

    for (Boid& currBoid : _boids)
    {
      for (std::size_t kk = 0; kk + 1 < currBoid.trail.size(); ++kk)
      {
        stackRenderer.pushThickTriangle3dLine(
          currBoid.trail.at(kk + 0).position,
          currBoid.trail.at(kk + 1).position,
          0.2f,
          0.2f,
          k_color,
          k_color
        );
      }
    }

    scene.stackRenderers.flush();
  }
}
