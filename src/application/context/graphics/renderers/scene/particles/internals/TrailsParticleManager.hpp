
#pragma once

// #include "geronimo/graphics/Geometry.hpp"
// #include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/containers/dynamic_heap_array.hpp"

#include <array>
#include <memory>
#include <vector>

class TrailsParticleManager {
public:
  TrailsParticleManager();

public:
  void update(float delta);
  void render(const gero::graphics::ICamera& inCamera);

public:
  void emitParticles(const glm::vec3& position, const glm::vec3& velocity);

private:
  struct TrailParticle {
    glm::vec3 position;
    glm::vec3 linearVelocity;
    float scale;
    glm::vec3 color;
    float life;
    float maxLife;

    static constexpr std::size_t trail_size = 2;
    std::array<glm::vec3, trail_size> trail;

    TrailParticle(
      const glm::vec3& position, const glm::vec3& linearVelocity, const glm::vec3& color, float scale, float life);
  };

  using TrailParticles = gero::dynamic_heap_array<TrailParticle>;

private:
  TrailParticles _trailParticles;
};
