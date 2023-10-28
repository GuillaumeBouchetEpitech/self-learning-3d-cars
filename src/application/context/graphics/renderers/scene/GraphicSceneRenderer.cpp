
#include "GraphicSceneRenderer.hpp"


void GraphicSceneRenderer::initialize(uint32_t width, uint32_t height, const glm::vec3& center, const glm::vec3& size)
{
  _camera.setPerspective(70.0f, 0.1f, 1500.0f);
  _camera.setSize(width, height);

  _flockingManager = AbstractFlockingManager::create();

  _stackRenderers.initialize();

  _modelsRenderer.initialize();
  _carTailsRenderer.initialize();

  // const auto& dimension = logic.circuitDimension;
  // const glm::vec3 boundariesSize = dimension.max - dimension.min;
  // _chessBoardFloorRenderer.initialize(dimension.center, boundariesSize);
  _chessBoardFloorRenderer.initialize(center, size);
  _backGroundTorusRenderer.initialize();
  _shapeStackRenderer.initialize();

}

void GraphicSceneRenderer::resize(uint32_t width, uint32_t height)
{
  _camera.setSize(width, height);
}

void GraphicSceneRenderer::computeMatrices()
{
  _camera.computeMatrices();
  setCamera(_camera);
}

void GraphicSceneRenderer::setCamera(const gero::graphics::ICamera& inCamera)
{
  const auto& matricesData = inCamera.getMatricesData();

  _stackRenderers.setMatricesData(matricesData);
  _carTailsRenderer.setMatricesData(matricesData);

  _shapeStackRenderer.setCamera(&inCamera);

  _particleManager.setCamera(&inCamera);
  _chessBoardFloorRenderer.setCamera(&inCamera);
  _backGroundTorusRenderer.setCamera(&inCamera);
  _animatedCircuitRenderer.setCamera(&inCamera);
  _modelsRenderer.setCamera(&inCamera);
}


void GraphicSceneRenderer::lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up)
{
  _camera.lookAt(eye, target, up);
}

gero::graphics::ICamera& GraphicSceneRenderer::getCamera() { return _camera; }

gero::graphics::IStackRenderers& GraphicSceneRenderer::getSceneStackRenderers() { return _stackRenderers; }
ParticleManager& GraphicSceneRenderer::getParticleManager() { return _particleManager; }
ChessBoardFloorRenderer& GraphicSceneRenderer::getChessBoardFloorRenderer() { return _chessBoardFloorRenderer; }
BackGroundTorusRenderer& GraphicSceneRenderer::getBackGroundTorusRenderer() { return _backGroundTorusRenderer; }
AnimatedCircuitRenderer& GraphicSceneRenderer::getAnimatedCircuitRenderer() { return _animatedCircuitRenderer; }
ModelsRenderer& GraphicSceneRenderer::getModelsRenderer() { return _modelsRenderer; }
AbstractFlockingManager& GraphicSceneRenderer::getFlockingManager() { return *_flockingManager; }
CarTailsRenderer& GraphicSceneRenderer::getCarTailsRenderer() { return _carTailsRenderer; }
ShapeStackRenderer& GraphicSceneRenderer::getShapeStackRenderer() { return _shapeStackRenderer; }
