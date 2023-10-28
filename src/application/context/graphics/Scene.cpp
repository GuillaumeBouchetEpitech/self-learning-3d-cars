
#include "Scene.hpp"

#include "application/states/StateManager.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/system/math/clamp.hpp"

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
Scene::initialize() {
  GlContext::enable(States::depthTest);
  GlContext::setDepthFunc(DepthFuncs::less);

  GlContext::enable(States::blend);
  GlContext::setBlendFunc(BlendFuncs::srcAlpha, BlendFuncs::oneMinusSrcAlpha);

  GlContext::disable(States::cullFace);
}

void
Scene::renderSimple() {
  Scene::_clear();
  Scene::updateMatrices();

  auto& graphic = Context::get().graphic;
  auto& hudRenderer = graphic.renderer.getHudRenderer();

  if (StateManager::get()->getState() != StateManager::States::Paused) {

    auto& postProcess = hudRenderer.getPostProcess();
    postProcess.startRecording();

    { // scene

      const glm::vec2 vSize = glm::vec2(hudRenderer.getCamera().getSize());

      GlContext::setViewport(0, 0, vSize.x, vSize.y);

      GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
      GlContext::clears(Buffers::color, Buffers::depth);
    }

    postProcess.stopRecording();
  }

  { // HUD

    const auto& matricesData = hudRenderer.getCamera().getMatricesData();
    hudRenderer.getPostProcess().setMatricesData(matricesData);
    hudRenderer.getStackRenderers().setMatricesData(matricesData);
    hudRenderer.getTextRenderer().setMatricesData(matricesData);

    Scene::_renderHUD();
  }

  gero::graphics::ShaderProgram::unbind();
}

void
Scene::renderAll() {
  Scene::_clear();
  Scene::updateMatrices();

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& sceneRenderer = graphic.renderer.getSceneRenderer();
  auto& hudRenderer = graphic.renderer.getHudRenderer();

  { // scene

    auto& postProcess = hudRenderer.getPostProcess();
    const gero::graphics::ICamera& camInstance = sceneRenderer.getCamera();
    const glm::vec2 vSize = glm::vec2(camInstance.getSize());

    postProcess.startRecording();

    Scene::renderScene(camInstance);

    postProcess.stopRecording();

    GlContext::setViewport(0, 0, vSize.x, vSize.y);
  }

  { // HUD

    const auto& matricesData = hudRenderer.getCamera().getMatricesData();
    hudRenderer.getPostProcess().setMatricesData(matricesData);
    hudRenderer.getStackRenderers().setMatricesData(matricesData);
    hudRenderer.getTextRenderer().setMatricesData(matricesData);

    Scene::_renderHUD();
  }

  gero::graphics::ShaderProgram::unbind();
}

void
Scene::updateMatrices() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& cameraData = graphic.cameraData;

  { // scene

    // gero::math::clamp vertical rotation [-70..+70]
    const float verticalLimit = glm::radians(70.0f);
    auto& rotations = cameraData.rotations;
    rotations.phi = glm::clamp(rotations.phi, -verticalLimit, verticalLimit);

    const float cosPhi = std::cos(rotations.phi);
    const glm::vec3 cameraDir = {
      cosPhi * std::cos(rotations.theta), cosPhi * std::sin(rotations.theta), std::sin(rotations.phi)};
    const glm::vec3 eye = cameraData.center + cameraDir * cameraData.distance;
    const glm::vec3 upAxis = {0.0f, 0.0f, 1.0f};

    graphic.renderer.getSceneRenderer().lookAt(eye, cameraData.center, upAxis);
    graphic.renderer.computeMatrices();

  } // scene
}

void
Scene::_clear() {
  const glm::vec2 vSize = Context::get().graphic.renderer.getHudRenderer().getCamera().getSize();

  GlContext::setViewport(0, 0, vSize.x, vSize.y);

  GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
  GlContext::clears(Buffers::color, Buffers::depth);
}
