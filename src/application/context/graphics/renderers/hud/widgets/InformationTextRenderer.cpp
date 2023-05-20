
#include "InformationTextRenderer.hpp"

#include "application/context/Context.hpp"

#include "application/context/graphics/renderers/hud/helpers/renderProgressBar.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/renderTextBackground.hpp"
#include "geronimo/graphics/advanced-concept/widgets/helpers/writeTime.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"

#include <iomanip>

void
InformationTextRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _alphaEasing = gero::easing::GenericEasing<2>().push(0.0f, _alpha, gero::easing::easeOutCubic).push(1.0f, 1.0f);
}

void
InformationTextRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _alphaEasing = gero::easing::GenericEasing<2>().push(0.0f, _alpha, gero::easing::easeInCubic).push(1.0f, 0.0f);
}

void
InformationTextRenderer::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _alpha = _alphaEasing.get(_timer.getCoefElapsed());
  }
}

void
InformationTextRenderer::render() {
  auto& context = Context::get();
  auto& logic = context.logic;
  auto& graphic = context.graphic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& vSize = graphic.cameraData.viewportSize;
  // auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec4 textColor = glm::vec4(0.8f, 0.8f, 0.8f, _alpha);
  const glm::vec4 textOutlineColor = glm::vec4(0.2f, 0.2f, 0.2f, _alpha);
  constexpr float k_textScale = 16.0f;
  constexpr float k_textHScale = k_textScale * 0.5f;
  constexpr float k_textDepth = 0.25f;

  { // top-center header text

    const glm::vec2 textPos = {vSize.x * 0.5, vSize.y - k_textScale - k_textHScale};

    textRenderer.setMainColor(textColor);
    textRenderer.setOutlineColor(textOutlineColor);
    textRenderer.setScale(k_textScale);
    textRenderer.setDepth(k_textDepth);
    textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::center);

    textRenderer.pushText(textPos, logic.hudText.header);

    gero::graphics::helpers::renderTextBackground(
      k_textDepth, glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f), glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f), 3.0f, 6.0f,
      graphic.hud.stackRenderers, textRenderer);

  } // top-center header text

  {

    const uint32_t totalCars = logic.cores.totalGenomes;
    const uint32_t liveCars = logic.simulation->getLiveGenomes();
    const uint32_t carsLeft = logic.simulation->getWaitingGenomes() + liveCars;

    const float progressValue = float(totalCars - carsLeft) / totalCars;

    {
      std::stringstream sstr;
      sstr << "live cars: " << liveCars;
      sstr << std::endl;
      sstr << "progress: " << (totalCars - carsLeft) << "/" << totalCars;
      const std::string str = sstr.str();

      const glm::vec2 textPos = {vSize.x * 0.5f, 4.0f * k_textScale - k_textHScale};

      textRenderer.setMainColor(textColor);
      textRenderer.setOutlineColor(textOutlineColor);
      textRenderer.setScale(k_textScale);
      textRenderer.setDepth(k_textDepth);
      textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::center);

      textRenderer.pushText(textPos, str);

      gero::graphics::helpers::renderTextBackground(
        k_textDepth, glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f), glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f), 3.0f,
        6.0f, graphic.hud.stackRenderers, textRenderer);
    }

    helpers::renderProgressBar(
      glm::vec2(vSize.x * 0.5f, 1.25f * k_textHScale), glm::vec2(250.0f, k_textScale * 1.5f), progressValue,
      k_textDepth, k_textScale, textColor, textOutlineColor, 4.0f, glm::vec4(1.0f, 1.0f, 1.0f, _alpha * 0.75f),
      glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f), glm::vec4(0.0f, 0.5f, 0.0f, _alpha * 0.75f));
  }

#if 0
  auto& performanceProfiler = context.logic.metrics.performanceProfiler;

  { // top-left performance stats

    std::stringstream sstr;

    {

      auto& timeDataMap = performanceProfiler.getTimeDataMap();

      std::array<std::string_view, 2> profilerNames = {{
        "Update",
        "Render",
      }};

      for (std::string_view& currName : profilerNames) {

        auto it = timeDataMap.find(currName.data());
        if (it != timeDataMap.end()) {

          auto& timeData = it->second;

          sstr << currName << ":";
          sstr << std::endl;

          gero::graphics::helpers::writeTime(sstr, timeData.getLatestDuration(), 0);
          sstr << std::endl;
          if (timeData.getAverageDuration() > 0) {
            sstr << "~";
            gero::graphics::helpers::writeTime(sstr, timeData.getAverageDuration(), 0);
            sstr << std::endl;
          }
          sstr << std::endl;
        }
      }
    }

    const std::string str = sstr.str();

    const glm::vec2 textPos = {
      k_textHScale, vSize.y - 5.0f * k_textScale - k_textHScale};

    textRenderer.setMainColor(textColor);
    textRenderer.setOutlineColor(textOutlineColor);
    textRenderer.setScale(k_textScale);
    textRenderer.setDepth(k_textDepth);
    textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::left);

    textRenderer.pushText(textPos, str);

    gero::graphics::helpers::renderTextBackground(
      k_textDepth, glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f),
      glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f), 3.0f, 6.0f,
      graphic.hud.stackRenderers, textRenderer);

  } // top-left performance stats

  { // top-right performance stats

    auto& timeDataMap = performanceProfiler.getTimeDataMap();

    auto it = timeDataMap.find("Frame");
    if (it != timeDataMap.end()) {

      auto& timeData = it->second;

      std::stringstream sstr;

      const int32_t latestFpsValue =
        int32_t(1000.0f / float(timeData.getLatestDuration()));

      {

        //
        //
        //

        sstr << "Frame:";
        sstr << std::endl;

        gero::graphics::helpers::writeTime(sstr, timeData.getLatestDuration(), 0);
        sstr << std::endl;
        if (timeData.getAverageDuration() > 0) {
          sstr << "~";
          gero::graphics::helpers::writeTime(sstr, timeData.getAverageDuration(), 0);
          sstr << std::endl;
        }
        sstr << std::endl;

        //
        //
        //

        sstr << "FPS:" << std::endl;
        sstr << "${1}" << latestFpsValue << std::endl;

        if (timeData.getAverageDuration() > 0) {
          const int32_t averageFpsValue =
            int32_t(1000.0f / float(timeData.getAverageDuration()));
          if (averageFpsValue > 0) {
            sstr << "${1}~" << averageFpsValue << std::endl;
          }
        }

        //
        //
        //
      }

      const glm::vec3 activeColor =
        (latestFpsValue < 25) ? glm::vec3(1, 0, 0) : glm::vec3(textColor);

      const std::string str = sstr.str();

      const glm::vec2 textPos = {
        vSize.x - k_textHScale, vSize.y - 3.0f * k_textScale - k_textHScale};

      textRenderer.setMainColor(textColor);
      textRenderer.setOutlineColor(textOutlineColor);
      textRenderer.setScale(k_textScale);
      textRenderer.setDepth(k_textDepth);
      textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::right);

      textRenderer.pushText(
        textPos, str,
        //
        gero::graphics::TextRenderer::State(textColor),
        gero::graphics::TextRenderer::State(glm::vec4(activeColor, _alpha)));

      gero::graphics::helpers::renderTextBackground(
        k_textDepth, glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f),
        glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f), 3.0f, 6.0f,
        graphic.hud.stackRenderers, textRenderer);
    }

  } // top-right performance stats
#endif
}
