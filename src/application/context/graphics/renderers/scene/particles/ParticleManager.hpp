
#pragma once

#include "internals/ExplosionParticlesManager.hpp"
#include "internals/TrailsParticleManager.hpp"

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/containers/dynamic_heap_array.hpp"

#include <array>
#include <memory>
#include <vector>

class ParticleManager {
public:
  ParticleManager() = default;

public:
  void update(float delta);
  void render(const gero::graphics::Camera& inCamera);

public:
  void emitParticles(const glm::vec3& position, const glm::vec3& velocity);

private:
  ExplosionParticlesManager _explosionParticlesManager;
  TrailsParticleManager _trailsParticleManager;
};
