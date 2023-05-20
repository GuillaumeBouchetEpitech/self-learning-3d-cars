
#include "ParticleManager.hpp"

void
ParticleManager::update(float delta) {

  _explosionParticlesManager.update(delta);
  _trailsParticleManager.update(delta);
}

void
ParticleManager::render(const gero::graphics::Camera& inCamera) {
  _explosionParticlesManager.render(inCamera);
  _trailsParticleManager.render(inCamera);
}

void
ParticleManager::emitParticles(const glm::vec3& position, const glm::vec3& velocity) {

  _explosionParticlesManager.emitParticles(position, velocity);
  _trailsParticleManager.emitParticles(position, velocity);
}
