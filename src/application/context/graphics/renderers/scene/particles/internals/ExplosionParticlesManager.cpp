
#include "ExplosionParticlesManager.hpp"

#include "application/context/Context.hpp"

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
    gero::rng::RNG::getRangedValue(-radius, radius), gero::rng::RNG::getRangedValue(-radius, radius),
    gero::rng::RNG::getRangedValue(-radius, radius));
}

}; // namespace

//
//
//

ExplosionParticlesManager::ExplosionParticle::ExplosionParticle(
  const glm::vec3& position, const glm::vec3& linearVelocity, const glm::vec3& color, float scale, float life)
  : position(position), linearVelocity(linearVelocity), scale(scale), color(color), life(life), maxLife(life) {}

ExplosionParticlesManager::ExplosionParticlesManager() {
  _smallExplosionParticles.pre_allocate(256);
  _bigExplosionParticles.pre_allocate(256);
}

void
ExplosionParticlesManager::update(float delta) {

  // update particle's life and handle removal of dead particles
  for (std::size_t ii = 0; ii < _smallExplosionParticles.size();) {
    _smallExplosionParticles.at(ii).life -= delta;
    if (_smallExplosionParticles.at(ii).life > 0.0f) {
      ++ii;
      continue;
    }

    _smallExplosionParticles.unsorted_erase(ii);
  }

  // update particle's life and handle removal of dead particles
  for (std::size_t ii = 0; ii < _bigExplosionParticles.size();) {
    _bigExplosionParticles.at(ii).life -= delta;
    if (_bigExplosionParticles.at(ii).life > 0.0f) {
      ++ii;
      continue;
    }

    _bigExplosionParticles.unsorted_erase(ii);
  }

  for (auto& currParticle : _smallExplosionParticles) {
    // update current position
    currParticle.position += currParticle.linearVelocity * delta;

    // if (currParticle.position.z < 0.0f) {
    //   currParticle.position.z = 0.0f;

    //   // bounce off the ground
    //   currParticle.linearVelocity.z += 100.0f * delta;
    // } else {
    //   // apply fake gravity
    //   currParticle.linearVelocity.z -= 20.0f * delta;
    // }
  }

  for (auto& currParticle : _bigExplosionParticles) {
    // update current position
    currParticle.position += currParticle.linearVelocity * delta;

    // if (currParticle.position.z < 0.0f) {
    //   currParticle.position.z = 0.0f;

    //   // bounce off the ground
    //   currParticle.linearVelocity.z += 100.0f * delta;
    // } else {
    //   // apply fake gravity
    //   currParticle.linearVelocity.z -= 20.0f * delta;
    // }
  }
}

void
ExplosionParticlesManager::render(const gero::graphics::ICamera& inCamera) {

  auto& context = Context::get();
  auto& sceneRenderer = context.graphic.renderer.getSceneRenderer();

  GeometriesStackRenderer::GeometryInstance instance;
  instance.position = glm::vec3(0.0f);
  instance.orientation = glm::identity<glm::quat>();
  instance.scale = glm::vec3(0.2f);
  instance.color = glm::vec4(0.6f, 0.6f, 0.0f, 1.0f);
  instance.outlineValue = 1.0f;

  const auto& frustumCulling = inCamera.getFrustumCulling();

  for (auto& particle : _smallExplosionParticles) {

    if (!frustumCulling.sphereInFrustum(particle.position, particle.scale))
      continue;

    // update scale
    const float localScale = particle.life / particle.maxLife * particle.scale;

    instance.color = glm::vec4(particle.color, 1.0f);
    instance.position = particle.position;
    instance.scale = glm::vec3(localScale);
    sceneRenderer.getShapeStackRenderer().pushSphere(instance);
  }

  auto tmpEasing = gero::easing::GenericEasing<3>();
  // tmpEasing.push(0.0f, 0.75f, gero::easing::easeOutCubic);
  tmpEasing.push(0.00f, 0.75f);
  tmpEasing.push(0.75f, 1.0f);
  tmpEasing.push(1.00f, 0.0f);

  for (auto& particle : _bigExplosionParticles) {

    if (!frustumCulling.sphereInFrustum(particle.position, particle.scale))
      continue;

    // update scale
    const float tmpCoef = 1.0f - particle.life / particle.maxLife;

    float easedVal = tmpEasing.get(tmpCoef);

    // // const float tmpCoef = particle.life / particle.maxLife;
    // const float coef = tmpCoef < 0.75f ? 1.0f : tmpCoef * 0.25f;

    instance.position = particle.position;
    instance.color = glm::vec4(particle.color, easedVal);
    // instance.scale = glm::vec3(particle.scale + (1.0f - tmpCoef) *
    // particle.scale * 0.2f);
    instance.scale = glm::vec3(particle.scale + easedVal);

    sceneRenderer.getShapeStackRenderer().pushSuperiorSphere(instance);
  }
}

void
ExplosionParticlesManager::emitParticles(const glm::vec3& position, const glm::vec3& velocity) {

  // const unsigned int totalParticles = gero::rng::RNG::getRangedValue(5, 8);

  const float maxVelLength = 10.0f;
  const float velLength = std::min(glm::length(velocity), maxVelLength);
  glm::vec3 normalizedVel(0);
  if (velLength > 0) {
    if (velLength < maxVelLength)
      normalizedVel = velocity / maxVelLength; // smaller than max velocity
    else
      normalizedVel = glm::normalize(velocity); // max velocity
  }

  // for (unsigned int ii = 0; ii < totalParticles; ++ii) {
  //   const glm::vec3 particlePos = position + getRandomVec3(1.0f);
  //   const glm::vec3 color = k_particleColors.at(gero::rng::RNG::getRangedValue(0, 3));

  //   const float maxVelocity = gero::rng::RNG::getRangedValue(15.0f, 25.0f);
  //   const glm::vec3 linearVelocity = glm::normalize(normalizedVel + getRandomVec3(0.25f)) * maxVelocity;

  //   const float scale = gero::rng::RNG::getRangedValue(0.5f, 1.5f);

  //   const float life = gero::rng::RNG::getRangedValue(0.5f, 1.5f);

  //   _smallExplosionParticles.emplace_back(particlePos, linearVelocity, color, scale, life);
  // }

  for (unsigned int ii = 0; ii < 3; ++ii) {
    const glm::vec3 particlePos = position + getRandomVec3(1.0f);

    const int colorIndex = gero::rng::RNG::getRangedValue(0, 3);
    const glm::vec3 color = k_particleColors.at(colorIndex);

    const glm::vec3 linearVelocity = glm::vec3(0, 0, 0);

    const float scale = gero::rng::RNG::getRangedValue(3.0f, 5.0f);

    const float life = gero::rng::RNG::getRangedValue(0.25f, 0.75f);

    _bigExplosionParticles.emplace_back(particlePos, linearVelocity, color, scale, life);
  }
}
