
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "geronimo/graphic/GlContext.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"
#include "geronimo/system/math/clamp.hpp"

void
Scene::initialise() {
  gero::graphic::GlContext::enable(gero::graphic::GlContext::States::depthTest);
  gero::graphic::GlContext::setDepthFunc(
    gero::graphic::GlContext::DepthFuncs::less);

  gero::graphic::GlContext::enable(gero::graphic::GlContext::States::blend);
  gero::graphic::GlContext::setBlendFunc(
    gero::graphic::GlContext::BlendFuncs::srcAlpha,
    gero::graphic::GlContext::BlendFuncs::oneMinuxSrcAlpha);

  gero::graphic::GlContext::disable(gero::graphic::GlContext::States::cullFace);
}

void
Scene::renderSimple() {
  Scene::_clear();
  Scene::updateMatrices();

  auto& graphic = Context::get().graphic;

  if (StateManager::get()->getState() != StateManager::States::Paused) {

    graphic.hud.postProcess.startRecording();

    { // scene

      const auto& vSize = graphic.cameraData.viewportSize;

      gero::graphic::GlContext::setViewport(0, 0, vSize.x, vSize.y);

      gero::graphic::GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
      gero::graphic::GlContext::clear(
        gero::asValue(gero::graphic::GlContext::Buffers::color) |
        gero::asValue(gero::graphic::GlContext::Buffers::depth));
    }

    graphic.hud.postProcess.stopRecording();
  }

  { // HUD

    const auto& matricesData = graphic.cameraData.hud.getMatricesData();
    graphic.hud.postProcess.setMatricesData(matricesData);
    graphic.hud.stackRenderers.wireframes.setMatricesData(matricesData);
    graphic.hud.stackRenderers.triangles.setMatricesData(matricesData);
    graphic.hud.textRenderer.setMatricesData(matricesData);

    Scene::_renderHUD();
  }

  gero::graphic::ShaderProgram::unbind();
}

void
Scene::renderAll() {
  Scene::_clear();
  Scene::updateMatrices();

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& cameraData = graphic.cameraData;

  { // scene

    const gero::graphic::Camera& camInstance = cameraData.scene;
    const auto& vSize = cameraData.viewportSize;

    graphic.hud.postProcess.startRecording();

    Scene::renderScene(camInstance);

    graphic.hud.postProcess.stopRecording();

    gero::graphic::GlContext::setViewport(0, 0, vSize.x, vSize.y);
  }

  { // HUD

    const auto& matricesData = cameraData.hud.getMatricesData();

    graphic.hud.postProcess.setMatricesData(matricesData);
    graphic.hud.stackRenderers.wireframes.setMatricesData(matricesData);
    graphic.hud.stackRenderers.triangles.setMatricesData(matricesData);
    graphic.hud.textRenderer.setMatricesData(matricesData);

    Scene::_renderHUD();
  }

  gero::graphic::ShaderProgram::unbind();
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
      cosPhi * std::cos(rotations.theta), cosPhi * std::sin(rotations.theta),
      std::sin(rotations.phi)};
    const glm::vec3 eye = cameraData.center + cameraDir * cameraData.distance;
    const glm::vec3 upAxis = {0.0f, 0.0f, 1.0f};

    cameraData.scene.setSize(
      cameraData.viewportSize.x, cameraData.viewportSize.y);
    cameraData.scene.lookAt(eye, cameraData.center, upAxis);
    cameraData.scene.computeMatrices();

    cameraData.hud.setOrthographic(
      0.0f, float(cameraData.viewportSize.x), 0.0f,
      float(cameraData.viewportSize.y), -10.0f, +10.0f);

    cameraData.hud.lookAt(
      glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    cameraData.hud.computeMatrices();

  } // scene
}

void
Scene::_clear() {
  const auto& viewportSize = Context::get().graphic.cameraData.viewportSize;

  gero::graphic::GlContext::setViewport(0, 0, viewportSize.x, viewportSize.y);

  gero::graphic::GlContext::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
  gero::graphic::GlContext::clear(
    gero::asValue(gero::graphic::GlContext::Buffers::color) |
    gero::asValue(gero::graphic::GlContext::Buffers::depth));
}
