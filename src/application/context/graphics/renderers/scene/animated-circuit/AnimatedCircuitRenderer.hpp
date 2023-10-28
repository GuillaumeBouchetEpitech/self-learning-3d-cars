
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/Texture.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

#include <array>
#include <memory>

class AnimatedCircuitRenderer {
public:
  struct AnimatedVertex {
    glm::vec3 postion;
    glm::vec3 color;
    glm::vec3 normal;
    glm::vec3 animatedNormal; // <= animation
    float limitId;            // <= animation

    AnimatedVertex() = default;

    AnimatedVertex(
      const glm::vec3& postion, const glm::vec3& color, const glm::vec3& normal, const glm::vec3& animatedNormal,
      float limitId)
      : postion(postion), color(color), normal(normal), animatedNormal(animatedNormal), limitId(limitId) {}
  };

  using AnimatedVertices = std::vector<AnimatedVertex>;

public:
  AnimatedCircuitRenderer() = default;
  ~AnimatedCircuitRenderer() = default;

public:
  void initialize(
    const std::vector<glm::vec3>& skeletonVertices, const AnimatedVertices& groundVertices,
    const AnimatedVertices& wallsVertices, float maxUpperValue);

public:
  void setCamera(const gero::graphics::ICamera* inCamera);

public:
  void update(float elapsedTime);
  void renderWireFrame();
  void renderWalls();
  void renderGround();

private:
  std::shared_ptr<gero::graphics::IUnboundShaderProgram> _shaderWireFrame;
  std::shared_ptr<gero::graphics::IUnboundShaderProgram> _shaderCircuitLit;
  std::shared_ptr<gero::graphics::IUnboundShaderProgram> _shaderCircuit;

  struct Geometries {
    gero::graphics::Geometry skeleton;
    gero::graphics::Geometry grounds;
    gero::graphics::Geometry walls;
  } _geometries;

  std::vector<uint32_t> _circuitIndices;

  float _targetValue = 10.0f;
  float _lowerValue = 10.0f;
  float _upperValue = 10.0f;
  float _maxUpperValue = 0.0f;
  std::size_t _maxPrimitiveCount = 0;

  const gero::graphics::ICamera* _camera = nullptr;

};
