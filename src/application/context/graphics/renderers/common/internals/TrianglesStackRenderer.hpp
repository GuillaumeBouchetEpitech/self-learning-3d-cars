
#pragma once

#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>

class ITrianglesStackRenderer {

public:
  virtual void pushTriangle(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
    const glm::vec4& colorA, const glm::vec4& colorB, const glm::vec4& colorC) = 0;

  virtual void pushTriangle(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
    const glm::vec4& color) = 0;

public:
  virtual void pushQuad(
    const glm::vec2& center, const glm::vec2& size, const glm::vec4& color,
    float z) = 0;
  virtual void pushQuad(
    const glm::vec2& center, const glm::vec2& size, const glm::vec4& color) = 0;

public:
  virtual void pushCircle(
    const glm::vec2& center, float radius, const glm::vec4& color, float z) = 0;
  virtual void
  pushCircle(const glm::vec2& center, float radius, const glm::vec4& color) = 0;

public:
  virtual void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB,
    float z) = 0;
  virtual void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB) = 0;
  virtual void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thickness,
    const glm::vec4& color, float z) = 0;
  virtual void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thickness,
    const glm::vec4& color) = 0;

public:
  virtual void pushThickTriangle3dLine(
    const glm::vec3& posA, const glm::vec3& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB) = 0;
  virtual void pushThickTriangle3dLine(
    const glm::vec3& posA, const glm::vec3& posB, float thickness,
    const glm::vec4& color) = 0;


};

struct TrianglesStackRenderer : public ITrianglesStackRenderer {

public:
  TrianglesStackRenderer() = default;
  ~TrianglesStackRenderer() = default;

public:
  void initialize(gero::graphics::ShaderProgram& shader, GeometriesAliases geometryId);

public:
  void pushTriangle(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
    const glm::vec4& colorA, const glm::vec4& colorB, const glm::vec4& colorC) override;

  void pushTriangle(
    const glm::vec3& posA, const glm::vec3& posB, const glm::vec3& posC,
    const glm::vec4& color) override;

public:
  void pushQuad(
    const glm::vec2& center, const glm::vec2& size, const glm::vec4& color,
    float z) override;
  void pushQuad(
    const glm::vec2& center, const glm::vec2& size, const glm::vec4& color) override;

public:
  void pushCircle(
    const glm::vec2& center, float radius, const glm::vec4& color, float z) override;
  void
  pushCircle(const glm::vec2& center, float radius, const glm::vec4& color) override;

public:
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB,
    float z) override;
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB) override;
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thickness,
    const glm::vec4& color, float z) override;
  void pushThickTriangle2dLine(
    const glm::vec2& posA, const glm::vec2& posB, float thickness,
    const glm::vec4& color) override;

public:
  void pushThickTriangle3dLine(
    const glm::vec3& posA, const glm::vec3& posB, float thicknessA,
    float thicknessB, const glm::vec4& colorA, const glm::vec4& colorB) override;
  void pushThickTriangle3dLine(
    const glm::vec3& posA, const glm::vec3& posB, float thickness,
    const glm::vec4& color) override;

public:
  void flush();

public:
  bool canRender() const;

private:
  struct Vertex {
    glm::vec3 position;
    glm::vec4 color;

    Vertex(const glm::vec3& position, const glm::vec4& color)
      : position(position), color(color) {}
  };

  using Vertices = std::vector<Vertex>;

private:
  gero::graphics::Geometry _geometry;
  Vertices _vertices;
};
