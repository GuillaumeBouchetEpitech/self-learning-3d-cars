
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/containers/dynamic_heap_array.hpp"

#include <array>
#include <memory>
#include <vector>

class SphereParticleManager {
private:
  struct ExplosionParticle {
    glm::vec3 position;
    glm::vec3 linearVelocity;
    float scale;
    glm::vec3 color;
    float life;
    float maxLife;

    ExplosionParticle(
      const glm::vec3& position, const glm::vec3& linearVelocity,
      const glm::vec3& color, float scale, float life);
  };

  // struct TrailParticle : public ExplosionParticle {
  //   static constexpr std::size_t trail_size = 2;
  //   std::array<glm::vec3, trail_size> trail;

  //   TrailParticle(
  //     const glm::vec3& position, const glm::vec3& linearVelocity,
  //     const glm::vec3& color, float scale, float life);
  // };

  using ExplosionParticles = gero::dynamic_heap_array<ExplosionParticle>;
  // using TrailParticles = gero::dynamic_heap_array<TrailParticle>;

private:
  struct ParticleInstance {
    glm::vec3 position;
    float scale;
    glm::vec3 color;

    ParticleInstance(
      const glm::vec3& position, float scale, const glm::vec3& color)
      : position(position), scale(scale), color(color) {}
  };

  // TrailParticles _trailParticles;
  // ExplosionParticles _smallExplosionParticles;
  ExplosionParticles _bigExplosionParticles;

  // std::vector<ParticleInstance> _smallSphereInstances;
  std::vector<ParticleInstance> _bigSphereInstances;
  std::shared_ptr<gero::graphics::ShaderProgram> _shader;
  // gero::graphics::Geometry _geometrySmallSphere;
  gero::graphics::Geometry _geometryBigSphere;

  gero::graphics::Camera::MatricesData _matricesData;

public:
  SphereParticleManager() = default;

public:
  void initialise();
  void setMatricesData(const gero::graphics::Camera::MatricesData& matricesData);

public:
  void update(float delta);
  void render();

public:
  void emitParticles(const glm::vec3& position, const glm::vec3& velocity);
};
