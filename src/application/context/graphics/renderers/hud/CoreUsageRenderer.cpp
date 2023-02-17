
#include "CoreUsageRenderer.hpp"

#include "application/context/Context.hpp"
#include "helpers/writeTime.hpp"
#include "helpers/renderTextBackground.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"

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
  _position.y = 12.0f * k_textScale;

  _size = {150, 100};
}

void
CoreUsageRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>()
                  .push(0.0f, _position.x, gero::easing::easeOutCubic)
                  .push(1.0f, k_faceInX);

  _isVisible = true;
}

void
CoreUsageRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>()
                  .push(0.0f, _position.x, gero::easing::easeInCubic)
                  .push(1.0f, k_faceOutX);

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
  auto& graphic = context.graphic;

  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 whiteColor(0.8f, 0.8f, 0.8f);
  const glm::vec3 redColor(1.0f, 0.0f, 0.0f);

  const glm::vec3 borderPos = {_position.x, _position.y + k_textScale + k_textHScale, 0.1f};
  const glm::vec2 borderSize = {_size.x, _size.y - k_textScale * 3.0f};

  const auto& profileData = context.logic.cores.profileData;

  const uint32_t allTimeMaxDelta = profileData.getAllTimeMaxDelta();

  const float verticalSize =
    (std::ceil(float(allTimeMaxDelta) / k_divider)) * k_divider;

  { // background

    const glm::vec4 bgColor = allTimeMaxDelta > k_slowdownDelta
                                ? glm::vec4(0.5f, 0.0f, 0.0f, 0.75f)
                                : glm::vec4(0.0f, 0.0f, 0.0f, 0.75f);

    stackRenderers.triangles.pushQuad(
      glm::vec3(glm::vec2(borderPos) + borderSize * 0.5f, borderPos.z),
      borderSize, bgColor, -0.2f);
    stackRenderers.wireFrames.pushRectangle(
      borderPos, borderSize, whiteColor, -0.1f);

  } // background

  //
  //

  { // dividers

    for (float currDivider = k_divider; currDivider < verticalSize;
         currDivider += k_divider) {
      const float ratio = currDivider / verticalSize;

      stackRenderers.wireFrames.pushLine(
        borderPos + glm::vec3(0, borderSize.y * ratio, 0.0f),
        borderPos + glm::vec3(borderSize.x, borderSize.y * ratio, 0.0f),
        whiteColor);
    }

  } // dividers

  //
  //

  {

    const float widthStep = borderSize.x / profileData.getMaxStateHistory();

    for (std::size_t coreIndex = 0; coreIndex < profileData.getTotalCores();
         ++coreIndex)
      for (uint32_t statIndex = 0;
           statIndex + 1 < profileData.getMaxStateHistory(); ++statIndex) {
        const float prevDelta =
          float(profileData.getCoreHistoryData(coreIndex, statIndex + 0).delta);
        const float currDelta =
          float(profileData.getCoreHistoryData(coreIndex, statIndex + 1).delta);

        const float prevHeight = borderSize.y * prevDelta / verticalSize;
        const float currHeight = borderSize.y * currDelta / verticalSize;

        stackRenderers.wireFrames.pushLine(
          borderPos + glm::vec3((statIndex + 0) * widthStep, prevHeight, 0.0f),
          borderPos + glm::vec3((statIndex + 1) * widthStep, currHeight, 0.0f),
          prevDelta < k_slowdownDelta ? whiteColor : redColor,
          currDelta < k_slowdownDelta ? whiteColor : redColor);
      }
  }

  stackRenderers.wireFrames.flush();
  stackRenderers.triangles.flush();
}

void
CoreUsageRenderer::renderHudText() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& profileData = logic.cores.profileData;

  std::vector<TextRenderer::MessageRectangle> outRectangles;
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
    writeTime(sstr, profileData.getLatestTotalDelta());

    std::string str = sstr.str();

    const glm::vec2 textPos = currPos;
    const float textDepth = 0.25f;

    textRenderer.pushText(
      textPos,
      str,
      color,
      k_textScale,
      textDepth,
      outlineColor,
      TextRenderer::TextAlign::left
      );

    helpers::renderTextBackground(
      textDepth,
      glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
      glm::vec4(0.3f, 0.3f, 0.3f, 1.0f),
      3.0f,
      6.0f
    );

  }

}
