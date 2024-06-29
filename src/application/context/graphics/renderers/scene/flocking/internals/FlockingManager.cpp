
#include "FlockingManager.hpp"

#include "application/context/Context.hpp"

#include "geronimo/graphics/make-geometries/MakeGeometries.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/math/angles.hpp"
#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/math/lerp.hpp"
#include "geronimo/system/math/safe-normalize.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

FlockingManager::FlockingManager() {
  constexpr std::size_t totalEntities = 64;
  _allBoids.resize(totalEntities);

  for (std::size_t ii = 0; ii < _allBoids.size(); ++ii) {
    auto& currBoid = _allBoids.at(ii);
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

  _spatialIndexer.clear();
  _spatialIndexer.pre_allocate(_allBoids.size());
  for (Boid& currBoid : _allBoids)
    _spatialIndexer.push(currBoid.position, &currBoid);
  _spatialIndexer.sync();

  for (Boid& currBoid : _allBoids) {

    currBoid.acceleration = {0, 0, 0};

    const float distanceToTarget = glm::distance(currBoid.position, target);

    if (distanceToTarget > 30.0f) {
      currBoid.seek(target, 1.0f);
    } else if (distanceToTarget < 10.0f) {
      currBoid.flee(target, 3.0f);
    } else {
      currBoid.strafe(target, currBoid.horizontalAngle, currBoid.verticalAngle, 1.5f);
    }

    const auto& searchResult = _spatialIndexer.search(currBoid.position, 4.0f);

    currBoid.separate(searchResult, 2.0f);
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

  if (_allBoids.empty())
    return;

  auto& context = Context::get();
  auto& shapeStackRenderer = context.graphic.renderer.getSceneRenderer().getShapeStackRenderer();

  const glm::vec4 k_color = glm::vec4(0.8f, 0.8f, 0.8f, 0.4f);

  GeometriesStackRenderer::GeometryInstance instance;
  instance.position = glm::vec3(0.0f);
  instance.orientation = glm::identity<glm::quat>();
  instance.scale = glm::vec3(0.2f);
  instance.color = k_color;
  instance.outlineValue = 0.0f;

  for (const Boid& currBoid : _allBoids) {
    for (const auto& currData : currBoid.graphicData.trailData) {
      instance.scale.x = currData.length;
      instance.orientation = currData.orientation;
      instance.position = currData.center;

      shapeStackRenderer.pushBox(instance);
    }
  }
}
