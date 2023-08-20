
#include "NewLeaderRenderer.hpp"

#include "application/context/Context.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/renderTextBackground.hpp"

void
NewLeaderRenderer::compute() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;

  if (logic.leaderCar.totalTimeAsLeader() > 1.0f)
    return;

  if (auto leaderData = logic.leaderCar.leaderData()) {
    const gero::graphics::Camera& scene = graphic.cameraData.scene;

    if (
      // we don't advertise a dead leader
      leaderData->isAlive &&
      // we don't advertise an early leader
      leaderData->fitness > 1.0f &&
      // we don't advertise a dying leader
      leaderData->groundSensor.value < 0.5f) {

      const glm::vec3 carPos = leaderData->liveTransforms.chassis.position;

      const bool isVisible = scene.sceneToHudCoord(carPos, _screenCoord);

      if (
        isVisible &&
        // out of range?
        _screenCoord.z < 1.0f) {
        _isVisible = true;
      }
    }
  }
}

void
NewLeaderRenderer::renderWireFrame() {
  if (!_isVisible)
    return;

  auto& graphic = Context::get().graphic;
  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 carPos = {_screenCoord.x, _screenCoord.y, 0.1f};
  const glm::vec3 textPos = carPos + glm::vec3(0, 100, 0.1f);

  stackRenderers.getWireFramesStack().pushLine(carPos, textPos, {1, 1, 1});
}

void
NewLeaderRenderer::renderHudText() {
  if (!_isVisible)
    return;

  auto& graphic = Context::get().graphic;
  // auto& stackRenderers = graphic.hud.stackRenderers;
  auto& textRenderer = graphic.hud.textRenderer;

  const float textScale = 16.0f;
  const glm::vec4 color = {0.8f, 0.8f, 0.8f, 1.0f};
  const glm::vec4 outlineColor = {0.3f, 0.3f, 0.0f, 1.0f};
  const float depth = 0.20f;

  const glm::vec2 textPos = glm::vec2(_screenCoord) + glm::vec2(0, 100.0f);

  const std::string_view message = "NEW\nLEADER";

  textRenderer.setMainColor(color);
  textRenderer.setOutlineColor(outlineColor);
  textRenderer.setScale(textScale);
  textRenderer.setDepth(depth);
  textRenderer.setHorizontalTextAlign(gero::graphics::TextRenderer::HorizontalTextAlign::center);
  textRenderer.setVerticalTextAlign(gero::graphics::TextRenderer::VerticalTextAlign::bottom);

  textRenderer.pushText(textPos, message.data());

  gero::graphics::helpers::renderTextBackground(
    depth, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), 3.0f, 6.0f, graphic.hud.stackRenderers,
    textRenderer);
}
