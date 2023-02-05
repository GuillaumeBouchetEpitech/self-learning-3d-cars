
#include "InformationTextRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "helpers/writeTime.hpp"
#include "helpers/renderTextBackground.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"

#include <iomanip>

void
InformationTextRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _alphaEasing = gero::easing::GenericEasing<2>()
                   .push(0.0f, _alpha, gero::easing::easeOutCubic)
                   .push(1.0f, 1.0f);
}

void
InformationTextRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _alphaEasing = gero::easing::GenericEasing<2>()
                   .push(0.0f, _alpha, gero::easing::easeInCubic)
                   .push(1.0f, 0.0f);
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

  auto& performanceProfiler = context.logic.metrics.performanceProfiler;

  const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _alpha);
  const glm::vec4 outlineColor = glm::vec4(0.2f, 0.2f, 0.2f, _alpha);
  constexpr float k_textScale = 16.0f;
  constexpr float k_textHScale = k_textScale * 0.5f;
  const float depth = 0.25f;

  { // top-center header text

    const glm::vec2 textPos = {vSize.x * 0.5, vSize.y - k_textScale - k_textHScale};
    textRenderer.pushText(textPos, logic.hudText.header, color, k_textScale, depth, outlineColor, TextRenderer::TextAlign::center);

    helpers::renderTextBackground(
      depth,
      glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f),
      glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f),
      3.0f,
      6.0f
    );

  } // top-center header text

  { // top-left performance stats

    std::stringstream sstr;

    {

      auto& timeDataMap = performanceProfiler.getTimeDataMap();

      std::array<std::string_view, 2> allProfleNames = {{
        "Update",
        "Render",
      }};

      for (std::string_view& currName : allProfleNames) {

        auto it = timeDataMap.find(currName.data());
        if (it != timeDataMap.end()) {

          auto& timeData = it->second;

          sstr << currName << ":";
          sstr << std::endl;

          sstr << writeTime(timeData.getLatestDuration(), 0) << std::endl;
          if (timeData.getAverageDuration() > 0) {
            sstr << "~" << writeTime(timeData.getAverageDuration(), 0);
            sstr << std::endl;
          }
          sstr << std::endl;
        }

      }

    }

    const std::string str = sstr.str();

    const glm::vec2 textPos = {k_textHScale, vSize.y - 5.0f * k_textScale - k_textHScale};

    textRenderer.pushText(textPos, str, color, k_textScale, depth, outlineColor);

    helpers::renderTextBackground(
      depth,
      glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f),
      glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f),
      3.0f,
      6.0f
    );

  } // top-left performance stats

  { // top-right performance stats

    auto& timeDataMap = performanceProfiler.getTimeDataMap();

    auto it = timeDataMap.find("Frame");
    if (it != timeDataMap.end()) {

      auto& timeData = it->second;

      std::stringstream sstr;

      const int32_t latestFpsValue = int32_t(1000000.0f / float(timeData.getLatestDuration()));

      {

        //
        //
        //

        sstr << "Frame:";
        sstr << std::endl;

        sstr << writeTime(timeData.getLatestDuration(), 0) << std::endl;
        if (timeData.getAverageDuration() > 0) {
          sstr << "~" << writeTime(timeData.getAverageDuration(), 0);
          sstr << std::endl;
        }
        sstr << std::endl;

        //
        //
        //

        sstr << "FPS:" << std::endl;
        sstr << "${1}" << latestFpsValue << "${0}/30" << std::endl;

        if (timeData.getAverageDuration() > 0) {
          const int32_t averageFpsValue = int32_t(1000000.0f / float(timeData.getAverageDuration()));
          if (averageFpsValue > 0) {
            sstr << "${1}~" << averageFpsValue << "${1}/30" << std::endl;
          }
        }

        //
        //
        //

      }


      const glm::vec3 activeColor = (latestFpsValue < 25) ? glm::vec3(1,0,0) : glm::vec3(color);


      const std::string str = sstr.str();

      const glm::vec2 textPos = {vSize.x - k_textHScale, vSize.y - 3.0f * k_textScale - k_textHScale};
      textRenderer.pushText(
        textPos,
        str,
        color,
        k_textScale,
        depth,
        outlineColor,
        TextRenderer::TextAlign::right,
        //
        TextRenderer::State(color),
        TextRenderer::State(glm::vec4(activeColor,_alpha))
        );

      helpers::renderTextBackground(
        depth,
        glm::vec4(0.0f, 0.0f, 0.0f, _alpha * 0.75f),
        glm::vec4(0.3f, 0.3f, 0.3f, _alpha * 0.75f),
        3.0f,
        6.0f
      );

    }

  } // top-right performance stats
}
