
#pragma once

#include "internals/TrianglesStackRenderer.hpp"
#include "internals/WireFramesStackRenderer.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

class StackRenderers
{
public:
  StackRenderers() = default;
  ~StackRenderers() = default;

public:
  void initialize(ShadersAliases shaderId, GeometriesAliases trianglesGeomId, GeometriesAliases wireFramesGeomId);

public:
  void setMatricesData(const gero::graphics::Camera::MatricesData& matricesData);

public:
  void flush();

public:
  ITrianglesStackRenderer& getTrianglesStack();
  IWireFramesStackRenderer& getWireFramesStack();

private:

  std::shared_ptr<gero::graphics::ShaderProgram> _shader;

  gero::graphics::Camera::MatricesData _matricesData;

  TrianglesStackRenderer _trianglesStackRenderer;
  WireFramesStackRenderer _wireFramesStackRenderer;

};
