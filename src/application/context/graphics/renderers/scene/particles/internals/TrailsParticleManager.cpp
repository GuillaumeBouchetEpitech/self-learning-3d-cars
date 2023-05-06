
#include "TrailsParticleManager.hpp"

#include "application/context/Context.hpp"
// #include "application/context/graphics/graphicsAliases.hpp"

// #include "geronimo/graphics/make-geometries/MakeGeometries.hpp"
// #include "geronimo/system/asValue.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

//
//
//

namespace /*anonymous*/
{

std::array<const glm::vec3, 3> k_particleColors{{
  {1.0f, 0.0f, 0.0f}, // red
  {1.0f, 1.0f, 0.0f}, // yellow
  {1.0f, 0.5f, 0.0f}, // orange
}};

glm::vec3
getRandomVec3(float radius) {
  return glm::vec3(
    gero::rng::RNG::getRangedValue(-radius, radius),
    gero::rng::RNG::getRangedValue(-radius, radius),
    gero::rng::RNG::getRangedValue(-radius, radius));
}

}; // namespace

//
//
//

TrailsParticleManager::TrailParticle::TrailParticle(
  const glm::vec3& position, const glm::vec3& linearVelocity,
  const glm::vec3& color, float scale, float life)
  : position(position), linearVelocity(linearVelocity), scale(scale),
    color(color), life(life), maxLife(life) {
  // initialize the particle's trail
  for (auto& trailPos : trail)
    trailPos = position;
}

TrailsParticleManager::TrailsParticleManager() {
  _trailParticles.pre_allocate(2048);
}

void
TrailsParticleManager::update(float delta) {
  // update particle's life and handle removal of dead particles
  for (std::size_t ii = 0; ii < _trailParticles.size();) {
    _trailParticles.at(ii).life -= delta;
    if (_trailParticles.at(ii).life > 0.0f) {
      ++ii;
      continue;
    }

    _trailParticles.unsorted_erase(ii);
  }

  for (auto& trailParticle : _trailParticles) {
    // make a trail by reusing the previous positions N times
    auto& trail = trailParticle.trail;
    for (std::size_t ii = trail.size() - 1; ii > 0; --ii)
      trail.at(ii) = trail.at(ii - 1);
    trail.at(0) = trailParticle.position;

    // update current position
    trailParticle.position += trailParticle.linearVelocity * delta;

    if (trailParticle.position.z < 0.0f) {
      trailParticle.position.z = 0.0f;

      // bounce off the ground
      trailParticle.linearVelocity.z += 100.0f * delta;
    } else {
      // apply fake gravity
      trailParticle.linearVelocity.z -= 20.0f * delta;
    }
  }
}

void
TrailsParticleManager::render(const gero::graphics::Camera& inCamera) {

  auto& context = Context::get();
  auto& scene = context.graphic.scene;

  const auto& frustumCulling = inCamera.getFrustumCulling();

  {
    auto& stackRenderer = scene.stackRenderers.getTrianglesStack();

    for (auto& trailParticle : _trailParticles) {

      if (!frustumCulling.sphereInFrustum(trailParticle.position, 10.0f))
        continue;

      // update scale
      const float localScale =
        trailParticle.life / trailParticle.maxLife * trailParticle.scale;

      const glm::vec4 particleColor = glm::vec4(trailParticle.color, 1);

      const auto& trail = trailParticle.trail;

      for (std::size_t ii = 0; ii + 1 < trail.size(); ++ii) {
        stackRenderer.pushThickTriangle3dLine(
          trail.at(ii + 0), trail.at(ii + 1), localScale * 0.5f, particleColor);
      }
      stackRenderer.pushThickTriangle3dLine(
        trailParticle.position, trail.at(0), localScale * 0.5f, particleColor);
    }
  }
}

void
TrailsParticleManager::emitParticles(
  const glm::vec3& position, const glm::vec3& velocity) {
  const unsigned int totalParticles = gero::rng::RNG::getRangedValue(5, 8);

  const float maxVelLength = 10.0f;
  const float velLength = std::min(glm::length(velocity), maxVelLength);
  glm::vec3 normalizedVel(0);
  if (velLength > 0) {
    if (velLength < maxVelLength)
      normalizedVel = velocity / maxVelLength; // smaller than max velocity
    else
      normalizedVel = glm::normalize(velocity); // max velocity
  }

  for (unsigned int ii = 0; ii < totalParticles; ++ii) {
    const float maxVelocity = gero::rng::RNG::getRangedValue(15.0f, 25.0f);
    const glm::vec3 particleVel =
      glm::normalize(normalizedVel + getRandomVec3(0.25f)) * maxVelocity;
    const glm::vec3 color =
      k_particleColors.at(gero::rng::RNG::getRangedValue(0, 3));
    const float scale = gero::rng::RNG::getRangedValue(0.5f, 1.5f);
    const float life = gero::rng::RNG::getRangedValue(0.5f, 1.5f);

    _trailParticles.emplace_back(position, particleVel, color, scale, life);
  }
}
