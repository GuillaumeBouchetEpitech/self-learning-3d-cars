
#pragma once

#include "demo/logic/graphicIds.hpp"

#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"
#include "geronimo/graphic/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>

struct WireframesStackRenderer {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec4 color;

    Vertex(const glm::vec3& position, const glm::vec4& color)
      : position(position), color(color) {}
  };
  using Vertices = std::vector<Vertex>;

public:
  WireframesStackRenderer() = default;
  ~WireframesStackRenderer() = default;

public:
  void initialise(ShaderIds shaderId, GeometryIds geometryId);
  void setMatricesData(const gero::graphic::Camera::MatricesData& matricesData);

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
  std::shared_ptr<gero::graphic::ShaderProgram> _shader;
  gero::graphic::Geometry _geometry;
  Vertices _vertices;

  gero::graphic::Camera::MatricesData _matricesData;
};
