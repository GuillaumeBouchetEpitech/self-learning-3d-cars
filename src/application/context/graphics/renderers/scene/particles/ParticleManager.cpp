
#include "ParticleManager.hpp"

void
ParticleManager::setCamera(const gero::graphics::ICamera* inCamera)
{
  _camera = inCamera;
}

void
ParticleManager::update(float delta) {

  _explosionParticlesManager.update(delta);
  // _trailsParticleManager.update(delta);
}

void
ParticleManager::render() {
  if (!_camera)
    D_THROW(std::runtime_error, "camera not setup");

  _explosionParticlesManager.render(*_camera);
  // _trailsParticleManager.render(_camera);
}

void
ParticleManager::emitParticles(const glm::vec3& position, const glm::vec3& velocity) {

  _explosionParticlesManager.emitParticles(position, velocity);
  // _trailsParticleManager.emitParticles(position, velocity);
}
