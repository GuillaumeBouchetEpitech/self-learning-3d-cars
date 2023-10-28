
#pragma once

#include "animated-circuit/AnimatedCircuitRenderer.hpp"
#include "background-torus/BackGroundTorusRenderer.hpp"
#include "car-tails/CarTailsRenderer.hpp"
#include "chessboard-floor/ChessBoardFloorRenderer.hpp"
#include "flocking/AbstractFlockingManager.hpp"
#include "models-car/ModelsRenderer.hpp"
#include "particles/ParticleManager.hpp"
#include "scene-stack-renderers/SceneStackRenderers.hpp"
#include "shape-stack-renderer/ShapeStackRenderer.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

class IGraphicSceneRenderer {
public:
  virtual void lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up) = 0;
  virtual void setCamera(const gero::graphics::ICamera& inCamera) = 0;

public:
  virtual gero::graphics::ICamera& getCamera() = 0;
  virtual gero::graphics::IStackRenderers& getSceneStackRenderers() = 0;
  virtual ParticleManager& getParticleManager() = 0;
  virtual ChessBoardFloorRenderer& getChessBoardFloorRenderer() = 0;
  virtual BackGroundTorusRenderer& getBackGroundTorusRenderer() = 0;
  virtual AnimatedCircuitRenderer& getAnimatedCircuitRenderer() = 0;
  virtual ModelsRenderer& getModelsRenderer() = 0;
  virtual AbstractFlockingManager& getFlockingManager() = 0;
  virtual CarTailsRenderer& getCarTailsRenderer() = 0;
  virtual ShapeStackRenderer& getShapeStackRenderer() = 0;
};

class GraphicSceneRenderer : public IGraphicSceneRenderer {
public:
  GraphicSceneRenderer() = default;
  ~GraphicSceneRenderer() = default;

public:
  void initialize(uint32_t width, uint32_t height, const glm::vec3& center, const glm::vec3& size);
  void resize(uint32_t width, uint32_t height);
  void computeMatrices();
  void setCamera(const gero::graphics::ICamera& inCamera) override;

public:
  void lookAt(const glm::vec3& eye, const glm::vec3& target, const glm::vec3& up) override;

public:
  gero::graphics::ICamera& getCamera() override;

  gero::graphics::IStackRenderers& getSceneStackRenderers() override;
  ParticleManager& getParticleManager() override;
  ChessBoardFloorRenderer& getChessBoardFloorRenderer() override;
  BackGroundTorusRenderer& getBackGroundTorusRenderer() override;
  AnimatedCircuitRenderer& getAnimatedCircuitRenderer() override;
  ModelsRenderer& getModelsRenderer() override;
  AbstractFlockingManager& getFlockingManager() override;
  CarTailsRenderer& getCarTailsRenderer() override;
  ShapeStackRenderer& getShapeStackRenderer() override;

private:
  gero::graphics::Camera _camera;

  SceneStackRenderers _stackRenderers;
  ParticleManager _particleManager;
  ChessBoardFloorRenderer _chessBoardFloorRenderer;
  BackGroundTorusRenderer _backGroundTorusRenderer;
  AnimatedCircuitRenderer _animatedCircuitRenderer;
  ModelsRenderer _modelsRenderer;
  std::unique_ptr<AbstractFlockingManager> _flockingManager;
  CarTailsRenderer _carTailsRenderer;
  ShapeStackRenderer _shapeStackRenderer;
};
