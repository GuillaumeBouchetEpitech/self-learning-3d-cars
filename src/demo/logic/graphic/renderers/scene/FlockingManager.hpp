
#pragma once

#include "geronimo/graphic/camera/Camera.hpp"

#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <memory>
#include <vector>

constexpr std::size_t k_trailSize = 15;

class FlockingManager {
private:
  struct Boid {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;

    std::array<glm::vec3, k_trailSize> trail;

    Boid();

    void seek(const glm::vec3& target, float coef);
    void separate(const glm::vec3& target, float coef);
    void separate(const std::vector<Boid>& boids, float radius, float coef);
    void wander(float coef);

    void applyAcceleration(float maxAcceleration, float maxVelocity);

    bool operator==(const Boid& other) const;
  };
  using Boids = std::vector<Boid>;

private:
  Boids _boids;

private:
  struct BufferAttr {
    glm::vec3 position;
    float scale;
    glm::vec3 color;

    BufferAttr(const glm::vec3& position, float scale, const glm::vec3& color)
      : position(position), scale(scale), color(color) {}
  };

  std::vector<BufferAttr> _particlesInstances;

  std::shared_ptr<gero::graphic::ShaderProgram> _shader;

  gero::graphic::Geometry _geometry;

  gero::graphic::Camera::MatricesData _matricesData;

public:
  FlockingManager();
  ~FlockingManager() = default;

public:
  void initialise();
  void setMatricesData(const gero::graphic::Camera::MatricesData& matricesData);

public:
  void update();

public:
  void render();
};