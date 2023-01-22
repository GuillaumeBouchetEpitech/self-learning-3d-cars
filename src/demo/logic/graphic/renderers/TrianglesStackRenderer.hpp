
#pragma once

#include "demo/logic/graphicIds.hpp"

#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"
#include "geronimo/graphic/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>

struct TrianglesStackRenderer {
public:
  struct Vertex {
    glm::vec3 position;
    glm::vec4 color;

    Vertex(const glm::vec3& position, const glm::vec4& color)
      : position(position), color(color) {}
  };
  using Vertices = std::vector<Vertex>;

public:
  TrianglesStackRenderer() = default;
  ~TrianglesStackRenderer() = default;

public:
  void initialise(ShaderIds shaderId, GeometryIds geometryId);
  void setMatricesData(const gero::graphic::Camera::MatricesData& matricesData);

public:
  void push(const Vertex& vertex);

  void pushTriangle(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
    const glm::vec4& colorA, const glm::vec4& colorB, const glm::vec4& colorC);

  void pushTriangle(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
    const glm::vec4& color);

public:
  void pushQuad(
    const glm::vec2& center, const glm::vec2& size, const glm::vec4& color,
    float z);
  void pushQuad(
    const glm::vec2& center, const glm::vec2& size, const glm::vec4& color);

public:
  void pushCircle(
    const glm::vec2& center, float radius, const glm::vec4& color, float z);
  void
  pushCircle(const glm::vec2& center, float radius, const glm::vec4& color);

public:
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB,
    float z);
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB);
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thickness,
    const glm::vec4& color, float z);
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thickness,
    const glm::vec4& color);

public:
  void pushThickTriangle3dLine(
    const glm::vec3& posA, const glm::vec3& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB);
  void pushThickTriangle3dLine(
    const glm::vec3& posA, const glm::vec3& posB, float thickness,
    const glm::vec4& color);

public:
  void flush();

  std::size_t getTrianglesCount() const;

private:
  std::shared_ptr<gero::graphic::ShaderProgram> _shader;
  gero::graphic::Geometry _geometry;
  Vertices _vertices;

  gero::graphic::Camera::MatricesData _matricesData;
};
