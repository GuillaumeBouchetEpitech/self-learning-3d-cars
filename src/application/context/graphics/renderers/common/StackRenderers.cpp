
#include "StackRenderers.hpp"

#include "application/context/Context.hpp"

#include "geronimo/system/asValue.hpp"

void StackRenderers::initialize(ShadersAliases shaderId, GeometriesAliases trianglesGeomId, GeometriesAliases wireFramesGeomId)
{
  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(gero::asValue(shaderId));

  _trianglesStackRenderer.initialize(*_shader, trianglesGeomId);
  _wireFramesStackRenderer.initialize(*_shader, wireFramesGeomId);
}

void StackRenderers::setMatricesData(const gero::graphics::Camera::MatricesData& matricesData)
{
  _matricesData = matricesData;
}

void StackRenderers::flush()
{
  if (!_trianglesStackRenderer.canRender() && !_wireFramesStackRenderer.canRender())
    return;

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);

  _trianglesStackRenderer.flush();
  _wireFramesStackRenderer.flush();
}

ITrianglesStackRenderer& StackRenderers::getTrianglesStack()
{
  return _trianglesStackRenderer;
}

IWireFramesStackRenderer& StackRenderers::getWireFramesStack()
{
  return _wireFramesStackRenderer;
}

