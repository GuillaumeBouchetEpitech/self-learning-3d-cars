
#include "GraphicsRenderer.hpp"

#include "geronimo/graphics/GlContext.hpp"

void
GraphicsRenderer::initialize(uint32_t width, uint32_t height, const glm::vec3& center, const glm::vec3& size) {
  D_MYLOG("MaxTextureSize: " << gero::graphics::GlContext::getMaxTextureSize());

  _sceneRenderer.initialize(width, height, center, size);
  _hudRenderer.initialize(width, height);
}

void
GraphicsRenderer::resize(uint32_t width, uint32_t height) {
  _sceneRenderer.resize(width, height);
  _hudRenderer.resize(width, height);
}

void
GraphicsRenderer::computeMatrices() {
  _sceneRenderer.computeMatrices();
  _hudRenderer.computeMatrices();
}

IGraphicSceneRenderer&
GraphicsRenderer::getSceneRenderer() {
  return _sceneRenderer;
}

IGraphicHudRenderer&
GraphicsRenderer::getHudRenderer() {
  return _hudRenderer;
}
