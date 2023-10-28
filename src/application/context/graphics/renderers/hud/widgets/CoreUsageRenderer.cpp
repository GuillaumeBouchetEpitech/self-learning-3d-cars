
#include "CoreUsageRenderer.hpp"

#include "application/context/Context.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/renderTextBackground.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/writeTime.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/math/BSpline.hpp"

namespace {

constexpr uint32_t k_slowdownDelta = 33;
constexpr float k_divider = 5.0f; // 5ms

constexpr float k_faceInX = +8.0f;
constexpr float k_faceOutX = -400.0f;

constexpr float k_textScale = 16.0f;
constexpr float k_textHScale = k_textScale * 0.5f;

} // namespace

CoreUsageRenderer::CoreUsageRenderer() {
  _position.x = k_faceOutX;
  _position.y = 13.0f * k_textScale;

  _size = {150, 100};
}

void
CoreUsageRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>();
  _moveEasing.push(0.0f, _position.x, gero::easing::easeOutCubic);
  _moveEasing.push(1.0f, k_faceInX);

  _isVisible = true;
}

void
CoreUsageRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>();
  _moveEasing.push(0.0f, _position.x, gero::easing::easeInCubic);
  _moveEasing.push(1.0f, k_faceOutX);

  _isVisible = false;
}

void
CoreUsageRenderer::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _position.x = _moveEasing.get(_timer.getCoefElapsed());
  }
}

void
CoreUsageRenderer::resize() {
  if (_isVisible)
    fadeIn(0.0f, 0.2f);
  else
    fadeOut(0.0f, 0.2f);
}

void
CoreUsageRenderer::renderWireFrame() {

  auto& context = Context::get();
  auto& renderer = context.graphic.renderer;
  auto& stackRenderers = renderer.getHudRenderer().getStackRenderers();

  const glm::vec3 whiteColor(0.8f, 0.8f, 0.8f);
  const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

  const glm::vec3 borderPos = {_position.x, _position.y + k_textHScale, 0.1f};
  const glm::vec2 borderSize = {_size.x, _size.y - k_textScale * 3.0f};

  const auto& profileData = context.logic.cores.profileData;

  const uint32_t allTimeMaxDelta = profileData.getAllTimeMaxDelta();

  const float verticalSize = (std::ceil(float(allTimeMaxDelta) / k_divider)) * k_divider;

  { // background

    const glm::vec4 bgColor =
      allTimeMaxDelta > k_slowdownDelta ? glm::vec4(0.5f, 0.0f, 0.0f, 0.75f) : glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);

    const glm::vec3 center = borderPos + glm::vec3(borderSize, 0) * 0.5f;

    stackRenderers.getTrianglesStack().pushQuad(center, borderSize, bgColor, -0.2f);
    stackRenderers.getWireFramesStack().pushRectangle(borderPos, borderSize, whiteColor, +0.1f);

  } // background

  //
  //

  { // dividers

    for (float currDivider = k_divider; currDivider < verticalSize; currDivider += k_divider) {
      const float ratio = currDivider / verticalSize;

      stackRenderers.getWireFramesStack().pushLine(
        borderPos + glm::vec3(0, borderSize.y * ratio, 0.0f),
        borderPos + glm::vec3(borderSize.x, borderSize.y * ratio, 0.0f), whiteColor * 0.5f);
    }

  } // dividers

  //
  //

  {

    const float widthStep = borderSize.x / profileData.getMaxStateHistory();

    for (std::size_t coreIndex = 0; coreIndex < profileData.getTotalCores(); ++coreIndex) {

      float prevDelta = float(profileData.getCoreHistoryData(coreIndex, 0).delta);
      float prevHeight = borderSize.y * prevDelta / verticalSize;

      for (uint32_t statIndex = 1; statIndex < profileData.getMaxStateHistory(); ++statIndex) {

        const float currDelta = float(profileData.getCoreHistoryData(coreIndex, statIndex).delta);
        const float currHeight = borderSize.y * currDelta / verticalSize;

        stackRenderers.getWireFramesStack().pushLine(
          borderPos + glm::vec3((statIndex + 0) * widthStep, prevHeight, 0.0f),
          borderPos + glm::vec3((statIndex + 1) * widthStep, currHeight, 0.0f),
          prevDelta < k_slowdownDelta ? whiteColor * 0.8f : redColor * 0.8f,
          currDelta < k_slowdownDelta ? whiteColor * 0.8f : redColor * 0.8f);

        prevDelta = currDelta;
        prevHeight = currHeight;
      }
    }
  }

  // {

  //   gero::math::BSpline bsplineSmoother;

  //   for (std::size_t coreIndex = 0; coreIndex < profileData.getTotalCores();
  //   ++coreIndex) {

  //     gero::math::BSpline::Definition smootherDef;
  //     smootherDef.degree = 4;
  //     smootherDef.dimensions = 1;
  //     smootherDef.knotsLength = profileData.getMaxStateHistory() *
  //     smootherDef.dimensions; smootherDef.getDataCallback = [&coreIndex,
  //     &profileData](uint32_t index)
  //     {
  //       return profileData.getCoreHistoryData(coreIndex, index).delta;
  //     };
  //     bsplineSmoother.initialize(smootherDef);

  //     constexpr unsigned int maxIterations = 100;
  //     constexpr float k_step = 1.0f / maxIterations; // tiny steps

  //     float prevCoef = k_step * 1.0f;
  //     float prevDelta = bsplineSmoother.calcAt(prevCoef, 0);

  //     for (float currCoef = k_step * 2.0f; currCoef < 1.0f; currCoef +=
  //     k_step) {

  //       const float currDelta = bsplineSmoother.calcAt(currCoef, 0);

  //       const float prevHeight = borderSize.y * prevDelta / verticalSize;
  //       const float currHeight = borderSize.y * currDelta / verticalSize;

  //       stackRenderers.getWireFramesStack().pushLine(
  //         borderPos + glm::vec3(prevCoef * borderSize.x, prevHeight, 0.1f),
  //         borderPos + glm::vec3(currCoef * borderSize.x, currHeight, 0.1f),
  //         prevDelta < k_slowdownDelta ? whiteColor : redColor,
  //         currDelta < k_slowdownDelta ? whiteColor : redColor);

  //       prevCoef = currCoef;
  //       prevDelta = currDelta;
  //     }
  //   }

  // }

  stackRenderers.flush();
}

void
CoreUsageRenderer::renderHudText() {

  auto& context = Context::get();
  auto& logic = context.logic;
  auto& profileData = logic.cores.profileData;
  auto& renderer = context.graphic.renderer;
  auto& textRenderer = renderer.getHudRenderer().getTextRenderer();
  auto& stackRenderers = renderer.getHudRenderer().getStackRenderers();

  std::vector<gero::graphics::TextRenderer::MessageRectangle> outRectangles;
  const glm::vec4 color = glm::vec4(0.8, 0.8, 0.8, 1);
  const glm::vec4 outlineColor = glm::vec4(0.3, 0.3, 0.3, 1);

  glm::vec2 currPos = _position;

  {
    std::stringstream sstr;

    const uint32_t totalCores = profileData.getTotalCores();
    if (totalCores == 1)
      sstr << "CORE: " << totalCores;
    else
      sstr << "CORES: " << totalCores;

    sstr << std::endl;
    sstr << "CPU time:" << std::endl;
    gero::graphics::helpers::writeTime(sstr, profileData.getLatestTotalDelta());

    std::string str = sstr.str();

    const glm::vec2 textPos = currPos;
    const float textDepth = 0.25f;

    textRenderer.setMainColor(color);
    textRenderer.setOutlineColor(outlineColor);
    textRenderer.setScale(k_textScale);
    textRenderer.setDepth(textDepth);
    // textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::left);
    textRenderer.setHorizontalTextAlign(gero::graphics::TextRenderer::HorizontalTextAlign::left);
    textRenderer.setVerticalTextAlign(gero::graphics::TextRenderer::VerticalTextAlign::top);

    textRenderer.pushText(textPos, str);

    gero::graphics::helpers::renderTextBackground(
      textDepth, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), 3.0f, 6.0f,
      stackRenderers, textRenderer);
  }
}
