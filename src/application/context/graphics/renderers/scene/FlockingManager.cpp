
#include "FlockingManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/make-geometries/MakeGeometries.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

FlockingManager::Boid::Boid() {
  position = {-200, 0, 0};
  position.x += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);
  position.y += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);
  position.z += gero::rng::RNG::getRangedValue(-10.0f, +10.0f);

  acceleration = {0, 0, 0};
  velocity = {0, 0, 0};

  for (auto& item : trail)
    item = position;
}

void
FlockingManager::Boid::seek(const glm::vec3& target, float coef) {
  glm::vec3 diff = target - position;
  const float diffMagnitude = glm::length(diff);

  if (diffMagnitude > 0.0f)
    diff = diff / diffMagnitude;

  acceleration += diff * coef;
}

void
FlockingManager::Boid::separate(const glm::vec3& target, float coef) {
  return seek(target, -coef);
}

void
FlockingManager::Boid::separate(
  const std::vector<Boid>& boids, float radius, float coef) {
  glm::vec3 total = {0, 0, 0};
  for (const Boid& other : boids) {
    if (*this == other)
      continue;

    const glm::vec3 diff = position - other.position;

    if (glm::length(diff) > radius)
      continue;

    total += diff;
  }

  const float magnitude = glm::length(total);
  if (magnitude > 0.0f)
    total = total / magnitude;

  acceleration += total * coef;
}

void
FlockingManager::Boid::wander(float coef) {
  acceleration.x += gero::rng::RNG::getRangedValue(-coef, +coef);
  acceleration.y += gero::rng::RNG::getRangedValue(-coef, +coef);
  acceleration.z += gero::rng::RNG::getRangedValue(-coef, +coef);
}

void
FlockingManager::Boid::applyAcceleration(
  float maxAcceleration, float maxVelocity, float elapsedTime) {
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

  bool needTrailUpdate = false;
  if (_timeUntilTrailUpdate > 0.0f)
    _timeUntilTrailUpdate -= elapsedTime;

  if (_timeUntilTrailUpdate <= 0.0f)
  {
    _timeUntilTrailUpdate = 1.0f / 30.0f;
    needTrailUpdate = true;
  }

  constexpr float maxAcc = 0.05f;
  constexpr float maxVel = 2.0f;

  for (Boid& boid : _boids) {
    boid.acceleration = {0, 0, 0};

    const float distance = glm::distance(boid.position, target);

    if (distance > 20.0f)
      boid.seek(target, 1.0f);
    else
      boid.separate(target, 3.0f);

    boid.separate(_boids, 1.5f, 2.0f);
    boid.wander(0.5f);

    constexpr float k_maxDistance = 100.0f;
    const float coef = 1.0f + 1.0f * distance / k_maxDistance;
    boid.applyAcceleration(maxAcc * coef, maxVel * coef, elapsedTime);

    if (needTrailUpdate)
    {
      // make a trail by reusing the previous positions N times
      for (std::size_t ii = boid.trail.size() - 1; ii > 0; --ii)
        boid.trail.at(ii) = boid.trail.at(ii - 1);
      boid.trail.at(0) = boid.position;
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
    instance.position = glm::vec3(0.0f),
    instance.orientation = glm::quat(0.0f, glm::vec3(0,0,1));
    instance.scale = glm::vec3(0.4f);
    instance.color = glm::vec4(0.6f, 0.6f, 0.0f, 1.0f);
    instance.outlineValue = 1.0f;

    for (Boid& boid : _boids)
    {
      instance.position = boid.position;
      scene.geometriesStackRenderer.pushAlias(1111, instance);
    }

  }

  {
    auto& stackRenderer = scene.stackRenderers.getTrianglesStack();

    // const glm::vec4 color = glm::vec4(0.6f, 0.6f, 0.0f, 0.2f);
    const glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);

    for (Boid& boid : _boids)
      for (std::size_t kk = 0; kk + 1 < boid.trail.size(); ++kk)
        stackRenderer.pushThickTriangle3dLine(
          boid.trail.at(kk + 0), boid.trail.at(kk + 1), 0.2f, color);

    scene.stackRenderers.flush();
  }

}