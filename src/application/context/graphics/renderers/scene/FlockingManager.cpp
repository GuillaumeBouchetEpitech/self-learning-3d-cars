
#include "FlockingManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GlContext.hpp"
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
  float maxAcceleration, float maxVelocity) {
  // limitate acc
  const float accMagnitude = glm::length(acceleration);
  if (accMagnitude > maxAcceleration)
    acceleration = (acceleration / accMagnitude) * maxAcceleration;
  velocity += acceleration;

  // limitate vel
  const float velMagnitude = glm::length(velocity);
  if (velMagnitude > maxVelocity)
    velocity = (velocity / velMagnitude) * maxVelocity;
  position += velocity;
}

bool
FlockingManager::Boid::operator==(const Boid& other) const {
  return this == &other;
}

FlockingManager::FlockingManager() {
  constexpr std::size_t totalEntities = 16;
  _boids.resize(totalEntities);
  _particlesInstances.reserve(totalEntities);
}

void
FlockingManager::initialise() {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(gero::asValue(ShadersAliases::particles));

  auto geoDef = resourceManager.getGeometryDefinition(
    gero::asValue(GeometriesAliases::particles));
  _geometry.initialise(*_shader, geoDef);

  gero::graphics::MakeGeometries::Vertices outVertices;
  outVertices.reserve(1024);
  gero::graphics::MakeGeometries::makeSphere(outVertices, 0, 0.5f);

  std::vector<glm::vec3> particlesVertices;
  particlesVertices.reserve(outVertices.size());
  for (const auto& vertex : outVertices)
    particlesVertices.push_back(vertex.position);

  _geometry.updateBuffer(0, particlesVertices);
  _geometry.setPrimitiveCount(particlesVertices.size());
}

void
FlockingManager::setMatricesData(
  const gero::graphics::Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void
FlockingManager::update() {
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

  constexpr float maxAcc = 0.1f;
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
    boid.applyAcceleration(maxAcc * coef, maxVel * coef);

    // make a trail by reusing the previous positions N times
    for (std::size_t ii = boid.trail.size() - 1; ii > 0; --ii)
      boid.trail.at(ii) = boid.trail.at(ii - 1);
    boid.trail.at(0) = boid.position;
  }
}

void
FlockingManager::render() {
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  if (_boids.empty())
    return;

  _particlesInstances.clear();
  for (Boid& boid : _boids)
    _particlesInstances.emplace_back(
      boid.position, 0.4f, glm::vec3(0.6f, 0.6f, 0.0f));

  if (!_particlesInstances.empty()) {
    _shader->bind();
    _shader->setUniform("u_composedMatrix", _matricesData.composed);

    _geometry.updateBuffer(1, _particlesInstances);
    _geometry.setInstancedCount(uint32_t(_particlesInstances.size()));
    _geometry.render();
  }

  // gero::graphics::GlContext::disable(gero::graphics::GlContext::States::depthTest);

  auto& stackRenderer = Context::get().graphic.scene.stackRenderers.triangles;

  // const glm::vec4 color = glm::vec4(0.6f, 0.6f, 0.0f, 0.2f);
  const glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.4f);

  for (Boid& boid : _boids)
    for (std::size_t kk = 0; kk + 1 < boid.trail.size(); ++kk)
      stackRenderer.pushThickTriangle3dLine(
        boid.trail.at(kk + 0), boid.trail.at(kk + 1), 0.2f, color);

  stackRenderer.flush();

  // gero::graphics::GlContext::enable(gero::graphics::GlContext::States::depthTest);
}
