
#pragma once

#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>

struct WireFramesStackRenderer {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec4 color;

    Vertex(const glm::vec3& position, const glm::vec4& color)
      : position(position), color(color) {}
  };
  using Vertices = std::vector<Vertex>;

public:
  WireFramesStackRenderer() = default;
  ~WireFramesStackRenderer() = default;

public:
  void initialise(ShadersAliases shaderId, GeometriesAliases geometryId);
  void setMatricesData(const gero::graphics::Camera::MatricesData& matricesData);

public:
  void push(const Vertex& vertex);
  void pushLine(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec4& colorA,
    const glm::vec4& colorB);
  void pushLine(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec4& color);
  void pushLine(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& colorA,
    const glm::vec3& colorB);
  void pushLine(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& color);

public:
  void
  pushCross(const glm::vec3& pos, const glm::vec3& color, float halfExtent);

public:
  void pushRectangle(
    const glm::vec2& pos, const glm::vec2& size, const glm::vec3& color,
    float depth = 0.0f);

public:
  void flush();

  std::size_t getLinesCount() const;

private:
  std::shared_ptr<gero::graphics::ShaderProgram> _shader;
  gero::graphics::Geometry _geometry;
  Vertices _vertices;

  gero::graphics::Camera::MatricesData _matricesData;
};
