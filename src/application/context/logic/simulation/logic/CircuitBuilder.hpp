
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"

#include <functional>
#include <string>
#include <vector>

class CircuitBuilder : gero::NonCopyable {
public:
  using Vec3Array = std::vector<glm::vec3>;
  using Indices = std::vector<int>;

  struct StartTransform {
    glm::vec3 position;
    glm::vec4 quaternion;
    glm::vec3 linearVelocity{0.0f, 0.0f, 0.0f};
  };

  struct Knot {
    glm::vec3 left;
    glm::vec3 right;
    float size;
    glm::vec3 color;
  };
  using Knots = std::vector<Knot>;
  using CircuitVertex = Knot;

public:
  using CallbackNoNormals = std::function<void(const Vec3Array& vertices, const Indices& indices)>;

  using CallbackNormals = std::function<void(
    const Vec3Array& vertices, const Vec3Array& colors, const Vec3Array& normals, const Indices& indices)>;

public:
  void parse(const std::string_view& filename);
  void load(const StartTransform& startTransform, const Knots& knots);

public:
  void generateWireFrameSkeleton(CallbackNoNormals onSkeletonPatch);
  void generateCircuitGeometry(
    CallbackNormals onNewGroundPatch, CallbackNormals onNewLeftWallPatch, CallbackNormals onNewRightWallPatch);

private:
  void generateSmoothedKnotsData(Knots& smoothedVertices);

public:
  const StartTransform& getStartTransform() const;
  const Knots& getKnots() const;

private:
  StartTransform _startTransform;
  Knots _knots;
};
