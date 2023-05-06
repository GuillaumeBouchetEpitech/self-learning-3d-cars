
#include "renderProgressBar.hpp"

#include "application/context/Context.hpp"

#include "geronimo/system/math/clamp.hpp"

namespace helpers {

void
renderProgressBar(
  const glm::vec2& inCenter, const glm::vec2& inSize, float inProgressValue,
  float inTextDepth, float inTextScale, const glm::vec4& inTextColor,
  const glm::vec4& inTextOutlineColor, float inBorderSize,
  const glm::vec4& inBorderColor, const glm::vec4& inBackgroundColor,
  const glm::vec4& inProgressColor) {

  const float progressValue = gero::math::clamp(inProgressValue, 0.0f, 1.0f);

  const float textHScale = inTextScale * 0.5f;

  auto& hud = Context::get().graphic.hud;
  auto& textRenderer = hud.textRenderer;
  auto& stackRenderers = hud.stackRenderers;

  std::stringstream sstr;
  sstr << uint32_t(progressValue * 100.0f) << "%";
  const std::string str = sstr.str();

  textRenderer.setMainColor(inTextColor);
  textRenderer.setOutlineColor(inTextOutlineColor);
  textRenderer.setScale(inTextScale);
  textRenderer.setDepth(inTextDepth);
  textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::center);

  textRenderer.pushText(inCenter, str);

  const glm::vec2 progressBarCenter = {inCenter.x, inCenter.y + textHScale};

  stackRenderers.getTrianglesStack().pushQuad(
    progressBarCenter, inSize + inBorderSize, inBorderColor,
    inTextDepth - 0.3f);

  //
  //

  stackRenderers.getTrianglesStack().pushQuad(
    progressBarCenter, inSize, inBackgroundColor, inTextDepth - 0.2f);

  //
  //

  glm::vec2 progressBarValSize = inSize;
  progressBarValSize.x *= progressValue;

  glm::vec2 progressBarValCenter = progressBarCenter;
  progressBarValCenter.x -= inSize.x * 0.5f;
  progressBarValCenter.x -= progressBarValSize.x * 0.5f;
  progressBarValCenter.x += inSize.x * progressValue;

  stackRenderers.getTrianglesStack().pushQuad(
    progressBarValCenter, progressBarValSize, inProgressColor,
    inTextDepth - 0.1f);
}

} // namespace helpers
