
#include "Scene.hpp"

#include "application/states/StateManager.hpp"

#include "renderers/hud/widgets/NewLeaderRenderer.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/renderTextBackground.hpp"
#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/math/clamp.hpp"

#include <iomanip>
#include <sstream>

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

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
    graphic.hud.widgets.fitnessDataRenderer.renderHudText();
    graphic.hud.widgets.coreUsageRenderer.renderHudText();

    newLeaderRndr.renderWireFrame();
    graphic.hud.widgets.informationTextRenderer.render();
    graphic.hud.widgets.coreUsageRenderer.renderWireFrame();
    graphic.hud.widgets.fitnessDataRenderer.renderWireFrame();
    graphic.hud.widgets.topologyRenderer.render();
    graphic.hud.widgets.leaderEyeRenderer.render();
    graphic.hud.widgets.screenTitles.render();

  } // wireFrames

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

      textRenderer.setMainColor(color);
      textRenderer.setOutlineColor(outlineColor);
      textRenderer.setScale(scale);
      textRenderer.setDepth(depth);
      textRenderer.setHorizontalTextAlign(gero::graphics::TextRenderer::HorizontalTextAlign::center);
      textRenderer.setVerticalTextAlign(gero::graphics::TextRenderer::VerticalTextAlign::center);

      textRenderer.pushText(textPos, message);

      gero::graphics::helpers::renderTextBackground(
        depth, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), 50.0f, 60.0f, stackRenderers,
        textRenderer);
    }

#endif

    if (currentState == StateManager::States::Paused) {

      const glm::vec4 color = glm::vec4(1);
      const glm::vec4 outlineColor = glm::vec4(0.4f, 0.4f, 0, 1);
      const float scale = 80.0f;
      const float depth = 0.75f;

      const glm::vec2 textPos = vSize * 0.5f;

      const std::string_view message = "PAUSED";

      textRenderer.setMainColor(color);
      textRenderer.setOutlineColor(outlineColor);
      textRenderer.setScale(scale);
      textRenderer.setDepth(depth);
      textRenderer.setHorizontalTextAlign(gero::graphics::TextRenderer::HorizontalTextAlign::center);
      textRenderer.setVerticalTextAlign(gero::graphics::TextRenderer::VerticalTextAlign::center);

      textRenderer.pushText(textPos, message);

      gero::graphics::helpers::renderTextBackground(
        depth, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), 50.0f, 60.0f, stackRenderers,
        textRenderer);
    }

  } // big titles

  stackRenderers.flush();
  textRenderer.render();
}

void
Scene::_renderHUD() {
  auto& context = Context::get();
  auto& graphic = context.graphic;

  GlContext::enable(States::depthTest);
  GlContext::disable(States::cullFace);

  graphic.hud.postProcess.render();

  Scene::_renderHUD_ortho();
  graphic.hud.widgets.thirdPersonCamera.render();
}
