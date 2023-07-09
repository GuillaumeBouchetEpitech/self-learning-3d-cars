
#pragma once

#include "geronimo/graphics/camera/Camera.hpp"

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <memory>
#include <vector>

constexpr std::size_t k_trailSize = 15;

class FlockingManager {
private:
  struct Boid {
    glm::vec3 position = {0,0,0};
    glm::vec3 velocity = {0,0,0};
    glm::vec3 acceleration = {0,0,0};

    float horizontalAngle = 0.0f;
    float verticalAngle = 0.0f;

    struct TrailData
    {
      glm::vec3 position;
    };

    std::array<TrailData, k_trailSize> trail;

    Boid();

    void seek(const glm::vec3& target, float coef);
    void flee(const glm::vec3& target, float coef);
    void separate(const std::vector<Boid>& boids, float radius, float coef);
    void strafe(const glm::vec3& target, float horizontalAngle, float verticalAngle, float coef);
    void wander(float coef);

    void updateTrail();

    void applyAcceleration(float maxAcceleration, float maxVelocity, float elapsedTime);

    bool operator==(const Boid& other) const;
  };
  using Boids = std::vector<Boid>;

private:
  Boids _boids;

  float _timeUntilTrailUpdate = 0.0f;

public:
  FlockingManager();
  ~FlockingManager() = default;

public:
  void update(float elapsedTime);
  void render();
};
