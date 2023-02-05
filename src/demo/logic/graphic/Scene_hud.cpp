
#include "Scene.hpp"

#include "demo/states/StateManager.hpp"

#include "renderers/hud/NewLeaderRenderer.hpp"
#include "renderers/hud/helpers/renderTextBackground.hpp"

#include "geronimo/graphic/GlContext.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/math/clamp.hpp"

#include <iomanip>
#include <sstream>

void
Scene::_renderHUD_ortho() {

  auto& graphic = Context::get().graphic;
  auto& stackRenderers = graphic.hud.stackRenderers;
  auto& textRenderer = graphic.hud.textRenderer;
  const auto& vSize = graphic.cameraData.viewportSize;

  textRenderer.clear();

  {
    NewLeaderRenderer newLeaderRndr;
    newLeaderRndr.compute();

    newLeaderRndr.renderHudText();
    graphic.hud.fitnessDataRenderer.renderHudText();
    graphic.hud.coreUsageRenderer.renderHudText();

    newLeaderRndr.renderWireframe();
    graphic.hud.informationTextRenderer.render();
    graphic.hud.coreUsageRenderer.renderWireframe();
    graphic.hud.fitnessDataRenderer.renderWireframe();
    graphic.hud.topologyRenderer.render();
    graphic.hud.leaderEyeRenderer.render();
    graphic.hud.screenTitles.render();

  } // wireframes

  { // big titles

    StateManager::States currentState = StateManager::get()->getState();

#if defined D_WEB_WEBWORKER_BUILD

    if (currentState == StateManager::States::WorkersLoading) {

      const glm::vec4 color = glm::vec4(1);
      const glm::vec4 outlineColor = glm::vec4(0.4f, 0.4f, 0, 1);
      const float scale = 30.0f;
      const float depth = 0.75f;

      const glm::vec2 textPos = vSize * 0.5f;

      std::stringstream sstr;
      sstr << "WEB WORKERS" << std::endl << "  LOADING  " << std::endl;
      const std::string message = sstr.str();

      textRenderer.pushText(textPos, message, color, scale, depth, outlineColor, TextRenderer::TextAlign::center);

      helpers::renderTextBackground(
        depth,
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
        50.0f,
        60.0f
      );

    }

#endif

    if (currentState == StateManager::States::Paused) {

      const glm::vec4 color = glm::vec4(1);
      const glm::vec4 outlineColor = glm::vec4(0.4f, 0.4f, 0, 1);
      const float scale = 80.0f;
      const float depth = 0.75f;

      const glm::vec2 textPos = vSize * 0.5f;

      const std::string message = "PAUSED";

      textRenderer.pushText(textPos, message, color, scale, depth, outlineColor, TextRenderer::TextAlign::center);

      helpers::renderTextBackground(
        depth,
        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
        glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
        50.0f,
        60.0f
      );

    }

  } // big titles

  stackRenderers.wireframes.flush();
  stackRenderers.triangles.flush();
  textRenderer.render();
}

void
Scene::_renderHUD() {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  gero::graphic::GlContext::enable(gero::graphic::GlContext::States::depthTest);
  gero::graphic::GlContext::disable(gero::graphic::GlContext::States::cullFace);

  graphic.hud.postProcess.render();

  Scene::_renderHUD_ortho();
  graphic.hud.thirdPersonCamera.render();
}
