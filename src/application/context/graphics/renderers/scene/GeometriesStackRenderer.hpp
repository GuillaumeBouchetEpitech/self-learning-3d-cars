
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/graphics/make-geometries/MakeGeometries.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

class GeometriesStackRenderer
{
public:
  struct GeometryInstance {
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    glm::vec4 color;
    glm::vec3 outlineColor;
  };
  using InstanceVertices = std::vector<GeometryInstance>;

private:
  std::shared_ptr<gero::graphics::ShaderProgram> _shader = nullptr;
  gero::graphics::Camera:: MatricesData _matricesData;


private:
  struct AliasedGeometry {
    gero::graphics::Geometry geometry;
    InstanceVertices instanceVertices;
  };

  std::unordered_map<int32_t, std::shared_ptr<AliasedGeometry>> _aliasedGeometriesMap;

public:
  GeometriesStackRenderer() = default;
  ~GeometriesStackRenderer() = default;

public:
  void initialise();
  void setMatricesData(const gero::graphics::Camera:: MatricesData& matricesData);

public:
  void createAlias(int32_t alias, const gero::graphics::MakeGeometries::Vertices& vertices);
  void deleteAlias(int32_t alias);
  void clearAlias(int32_t alias);
  void pushAlias(int32_t alias, const GeometryInstance& newInstance, float inForwardOffset = 0.0f);

public:
  void clearAll();
  void renderAll(const glm::vec3& inLightPos);

};
