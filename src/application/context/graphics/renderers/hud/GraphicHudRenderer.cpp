
#include "GraphicHudRenderer.hpp"

void
GraphicHudRenderer::initialize(uint32_t width, uint32_t height) {
  _camera.setOrthographic(0.0f, float(width), 0.0f, float(height), -10.0f, +10.0f);
  _camera.setSize(width, height);

  const glm::vec3 eye = {0.0f, 0.0f, 1.0f};
  const glm::vec3 center = {0.0f, 0.0f, 0.0f};
  const glm::vec3 upAxis = {0.0f, 1.0f, 0.0f};
  _camera.lookAt(eye, center, upAxis);

  _stackRenderers.initialize("./thirdparties/dependencies/geronimo/src");
  _textRenderer.initialize("./thirdparties/dependencies/geronimo/src");

  _postProcess.initialize(width, height);
  _postProcess.setGeometry(glm::vec2(0, 0), glm::vec2(width, height), -2.0f);

  _widgets.topologyRenderer.initialize();
  _widgets.thirdPersonCamera.initialize();
  _widgets.leaderEyeRenderer.initialize();
}

void
GraphicHudRenderer::resize(uint32_t width, uint32_t height) {
  _camera.setOrthographic(0.0f, float(width), 0.0f, float(height), -10.0f, +10.0f);
  _camera.setSize(width, height);

  _postProcess.resize(width, height);
  _postProcess.setGeometry(glm::vec2(0, 0), glm::vec2(width, height), -2.0f);

  _widgets.topologyRenderer.resize();
  _widgets.thirdPersonCamera.resize();
  _widgets.coreUsageRenderer.resize();
  _widgets.fitnessDataRenderer.resize();
  _widgets.leaderEyeRenderer.resize();
}

void
GraphicHudRenderer::computeMatrices() {
  _camera.computeMatrices();
}

gero::graphics::ICamera&
GraphicHudRenderer::getCamera() {
  return _camera;
}
gero::graphics::IStackRenderers&
GraphicHudRenderer::getStackRenderers() {
  return _stackRenderers;
}
gero::graphics::ITextRenderer&
GraphicHudRenderer::getTextRenderer() {
  return _textRenderer;
}

PostProcess&
GraphicHudRenderer::getPostProcess() {
  return _postProcess;
}

GraphicHudRenderer::Widgets&
GraphicHudRenderer::getWidgets() {
  return _widgets;
}
