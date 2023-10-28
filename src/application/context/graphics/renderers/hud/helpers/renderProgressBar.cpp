
#include "renderProgressBar.hpp"

#include "application/context/Context.hpp"

#include "geronimo/system/math/clamp.hpp"

namespace helpers {

void
renderProgressBar(
  const glm::vec2& inCenter, const glm::vec2& inSize, float inProgressValueA, float inProgressValueB, float inTextDepth,
  float inTextScale, const glm::vec4& inTextColor, const glm::vec4& inTextOutlineColor, float inBorderSize,
  const glm::vec4& inBorderColor, const glm::vec4& inBackgroundColor, const glm::vec4& inProgressColorA,
  const glm::vec4& inProgressColorB) {

  const float progressValueA = gero::math::clamp(inProgressValueA, 0.0f, 1.0f);
  const float progressValueB = gero::math::clamp(inProgressValueB, 0.0f, 1.0f);

  const float textHScale = inTextScale * 0.5f;

  auto& hudRenderer = Context::get().graphic.renderer.getHudRenderer();
  auto& textRenderer = hudRenderer.getTextRenderer();
  auto& stackRenderers = hudRenderer.getStackRenderers();

  std::stringstream sstr;
  sstr << uint32_t(progressValueA * 100.0f) << "%";
  const std::string str = sstr.str();

  textRenderer.setMainColor(inTextColor);
  textRenderer.setOutlineColor(inTextOutlineColor);
  textRenderer.setScale(inTextScale);
  textRenderer.setDepth(inTextDepth);
  textRenderer.setHorizontalTextAlign(gero::graphics::TextRenderer::HorizontalTextAlign::center);
  textRenderer.setVerticalTextAlign(gero::graphics::TextRenderer::VerticalTextAlign::center);

  const glm::vec2 progressBarCenter = {inCenter.x, inCenter.y + textHScale};

  textRenderer.pushText(progressBarCenter, str);

  stackRenderers.getTrianglesStack().pushQuad(
    progressBarCenter, inSize + inBorderSize, inBorderColor, inTextDepth - 0.3f);

  //
  //

  stackRenderers.getTrianglesStack().pushQuad(progressBarCenter, inSize, inBackgroundColor, inTextDepth - 0.2f);

  //
  //

  {
    glm::vec2 progressBarValSize = inSize;
    progressBarValSize.x *= progressValueA;

    glm::vec2 progressBarValCenter = progressBarCenter;
    progressBarValCenter.x -= inSize.x * 0.5f;
    progressBarValCenter.x -= progressBarValSize.x * 0.5f;
    progressBarValCenter.x += inSize.x * progressValueA;

    stackRenderers.getTrianglesStack().pushQuad(
      progressBarValCenter, progressBarValSize, inProgressColorA, inTextDepth - 0.1f);
  }

  {
    glm::vec2 progressBarValSize = inSize;
    progressBarValSize.x *= progressValueB;

    glm::vec2 progressBarValCenter = progressBarCenter;
    progressBarValCenter.x -= inSize.x * 0.5f;
    progressBarValCenter.x -= progressBarValSize.x * 0.5f;
    progressBarValCenter.x += inSize.x * progressValueB;

    stackRenderers.getTrianglesStack().pushQuad(
      progressBarValCenter, progressBarValSize, inProgressColorB, inTextDepth - 0.15f);
  }
}

} // namespace helpers
