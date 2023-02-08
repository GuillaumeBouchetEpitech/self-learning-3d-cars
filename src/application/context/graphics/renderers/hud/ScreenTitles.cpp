
#include "ScreenTitles.hpp"

#include "application/context/Context.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"

#include <iomanip>

namespace
{

  constexpr float k_timeAnimDivider = 0.3f;

  TextRenderer::State _getColor(
    const glm::vec4& inBaseOutlineColor,
    const glm::vec4& inAnimOutlineColor,
    float currVal,
    int step,
    int maxSteps
  ) {
    const float midVal = float(step) / float(maxSteps);
    const float minVal = midVal - k_timeAnimDivider;
    const float maxVal = midVal + k_timeAnimDivider;

    if (currVal > minVal && currVal < maxVal)
    {
      if (currVal < midVal)
      {
        const float subCoef = 1.0f - (currVal - minVal) * (1.0f / k_timeAnimDivider);
        return TextRenderer::State(
          std::nullopt,
          glm::mix(inAnimOutlineColor, inBaseOutlineColor, subCoef)
        );
      }
      else
      if (currVal >= midVal)
      {
        const float subCoef = 1.0f - ((currVal - midVal) * (1.0f / k_timeAnimDivider));
        return TextRenderer::State(
          std::nullopt,
          glm::mix(inBaseOutlineColor, inAnimOutlineColor, subCoef)
        );
      }
    }
    return TextRenderer::State(std::nullopt, inBaseOutlineColor);
  }

}

void
ScreenTitles::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  constexpr float step1 = 0.25f;
  constexpr float step2 = 0.50f;
  constexpr float step3 = 0.75f;

  _backgroundEasing = gero::easing::GenericEasing<2>()
                        .push(0.00f, _backgroundAlpha)
                        .push(step1, 1.0f);

  _mainTitleAlphaEasing = gero::easing::GenericEasing<2>()
                          .push(step1, _mainTitleAlpha, gero::easing::easeOutCubic)
                          .push(step2, 1.0f);

  _fitnessTitleAlphaEasing =
    gero::easing::GenericEasing<2>()
      .push(step2, _fitnessTitleAlpha, gero::easing::easeOutCubic)
      .push(step3, 1.0f);

  _commentTitleAlphaEasing =
    gero::easing::GenericEasing<2>()
      .push(step3, _commentTitleAlpha, gero::easing::easeOutCubic)
      .push(1.00f, 1.0f);

  //
  //
  //

  _titleAnimTimer.start(delay, duration * 1.8f);

  constexpr float k_minVal = 0.0f - k_timeAnimDivider;
  constexpr float k_maxVal = 1.0f + k_timeAnimDivider;

  _mainTitleAnimEasing =
    gero::easing::GenericEasing<2>()
      .push(step1, k_minVal)
      .push(1.0f, k_maxVal);

  _fitnessTitleAnimEasing =
    gero::easing::GenericEasing<2>()
      .push(step2, k_minVal)
      .push(1.0f, k_maxVal);

  _commentTitleAnimEasing =
    gero::easing::GenericEasing<2>()
      .push(step2, k_minVal)
      .push(1.0f, k_maxVal);


  //
  //
  //

}

void
ScreenTitles::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  constexpr float step1 = 0.2f;
  constexpr float step2 = 0.4f;
  constexpr float step3 = 0.6f;

  _mainTitleAlphaEasing = gero::easing::GenericEasing<2>()
                       .push(0.00f, _mainTitleAlpha)
                       .push(step1, 0.0f);

  _fitnessTitleAlphaEasing =
    gero::easing::GenericEasing<2>()
      .push(step1, _fitnessTitleAlpha, gero::easing::easeInCubic)
      .push(step2, 0.0f);

  _commentTitleAlphaEasing =
    gero::easing::GenericEasing<2>()
      .push(step2, _commentTitleAlpha, gero::easing::easeInCubic)
      .push(step3, 0.0f);

  _backgroundEasing =
    gero::easing::GenericEasing<2>()
      .push(step3, _backgroundAlpha, gero::easing::easeInCubic)
      .push(1.00f, 0.0f);
}

void
ScreenTitles::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);

    const float val = _timer.getCoefElapsed();

    _backgroundAlpha = _backgroundEasing.get(val);
    _mainTitleAlpha = _mainTitleAlphaEasing.get(val);
    _fitnessTitleAlpha = _fitnessTitleAlphaEasing.get(val);
    _commentTitleAlpha = _commentTitleAlphaEasing.get(val);
  }

  if (!_titleAnimTimer.isDone()) {
    _titleAnimTimer.update(elapsedTime);
  }

}

void
ScreenTitles::render() {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& textRenderer = graphic.hud.textRenderer;
  auto& vSize = graphic.cameraData.viewportSize;

  if (_backgroundAlpha > 0.0f) {

    graphic.hud.stackRenderers.triangles.pushQuad(
      glm::vec2(vSize * 0.5f), vSize,
      glm::vec4(0, 0, 0, _backgroundAlpha * 0.5f), 0.4f);
  }

  const float animTimeVal = _titleAnimTimer.getCoefElapsed();

  glm::vec2 currPos = vSize * 0.5f;

  const float depth = 0.5f;

  if (_mainTitleAlpha > 0.0f) {

    const float animEasingVal = _mainTitleAnimEasing.get(animTimeVal);

    std::stringstream sstr;
    sstr << "${1}G${2}e${3}n${4}e${5}r${6}a${7}t${8}i${9}o${10}n${11}: ";
    sstr << context.logic.simulation->getGenerationNumber();
    const std::string message = sstr.str();

    const glm::vec4 color = glm::vec4(glm::vec3(0.7f), _mainTitleAlpha);
    const glm::vec4 outlineColor = glm::vec4(glm::vec3(0.2f, 0.2f, 0.2f), _mainTitleAlpha);
    const glm::vec4 outlineColorTitleAnim = glm::vec4(glm::vec3(0.7f, 0.7f, 0.7f), _mainTitleAlpha);

    constexpr float k_scale = 50.0f;

    textRenderer.pushText(
      currPos,
      message,
      color,
      k_scale,
      depth,
      outlineColor,
      TextRenderer::TextAlign::center,
      //
      TextRenderer::State(color, outlineColor),
      //
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 1, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 2, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 3, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 4, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 5, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 6, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 7, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 8, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 9, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 10, 11),
      _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 11, 11)
      );
  }

  {

    constexpr float k_scale = 30.0f;

    const float prevFitness = context.logic.fitnessStats.get(-2);
    const float currFitness = context.logic.fitnessStats.get(-1);
    // const float prevFitness = 1.0f;
    // const float currFitness = 2.0f;

    if (currFitness > 0.0f) {

      currPos.y -= 55.0f;

      if (_fitnessTitleAlpha > 0.0f) {

        const float animEasingVal = _mainTitleAnimEasing.get(animTimeVal);

        std::stringstream sstr;
        sstr << "${1}F${2}i${3}t${4}n${5}e${6}s${7}s${8}: ";
        sstr << std::fixed << std::setprecision(1) << currFitness;
        std::string message = sstr.str();

        const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _fitnessTitleAlpha);
        const glm::vec4 outlineColor = glm::vec4(0.2f, 0.2f, 0.2f, _fitnessTitleAlpha);
        const glm::vec4 outlineColorTitleAnim = glm::vec4(glm::vec3(0.7f, 0.7f, 0.7f), _fitnessTitleAlpha);

        textRenderer.pushText(
          currPos,
          message,
          color,
          k_scale,
          depth,
          outlineColor,
          TextRenderer::TextAlign::center,
          //
          TextRenderer::State(color, outlineColor),
          //
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 1, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 2, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 3, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 4, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 5, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 6, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 7, 8),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 8, 8)
          );

      }

      currPos.y -= 35.0f;

      if (_commentTitleAlpha > 0.0f && currFitness != prevFitness) {

        const float animEasingVal = _commentTitleAnimEasing.get(animTimeVal);

        std::stringstream sstr;

        const float diff = currFitness - prevFitness;

        if (currFitness > prevFitness)
          sstr << "${1}Sm${2}a${3}r${4}t${5}e${6}r r${7}e${8}s${9}u${10}l${11}t${12} (+";
        else if (currFitness < prevFitness)
          sstr << "${1}W${2}o${3}r${4}s${5}e ${6}r${7}e${8}s${9}u${10}l${11}t${12} (";
        sstr << std::fixed << std::setprecision(1) << diff;
        sstr << ")";

        const std::string message = sstr.str();

        const glm::vec4 color = glm::vec4(0.8f, 0.8f, 0.8f, _commentTitleAlpha);
        const glm::vec4 outlineColorTitleAnim = glm::vec4(glm::vec3(0.7f, 0.7f, 0.7f), _fitnessTitleAlpha);

        glm::vec4 outlineColor = glm::vec4(0.0f, 0.0f, 0.0f, _commentTitleAlpha);
        if (currFitness > prevFitness)
          outlineColor.y = 0.5f;
        else if (currFitness < prevFitness)
          outlineColor.x = 0.5f;

        textRenderer.pushText(
          currPos,
          message,
          color,
          k_scale,
          depth,
          outlineColor,
          TextRenderer::TextAlign::center,
          //
          TextRenderer::State(color, outlineColor),
          //
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 1, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 2, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 3, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 4, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 5, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 6, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 7, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 8, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 9, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 10, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 11, 12),
          _getColor(outlineColor, outlineColorTitleAnim, animEasingVal, 12, 12)
          );

      }
    }
  }
}
