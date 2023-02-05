
#include "FitnessDataRenderer.hpp"

#include "demo/logic/Context.hpp"

#include "helpers/renderTextBackground.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/math/clamp.hpp"

#include <iomanip>

namespace {

constexpr float k_faceInX = +8.0f;
constexpr float k_faceOutX = -400.0f;

constexpr float k_textScale = 16.0f;
constexpr float k_textHScale = k_textScale * 0.5f;

} // namespace

FitnessDataRenderer::FitnessDataRenderer() {
  _position.x = k_faceOutX;
  _position.y = 60 + 2.0f * k_textScale + k_textHScale;

  _size = {150, 75};
}

void
FitnessDataRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>()
                  .push(0.0f, _position.x, gero::easing::easeOutCubic)
                  .push(1.0f, k_faceInX);
  _alphaEasing = gero::easing::GenericEasing<2>()
                   .push(0.0f, _alpha, gero::easing::easeOutCubic)
                   .push(1.0f, 1.0f);

  _isVisible = true;
}

void
FitnessDataRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>()
                  .push(0.0f, _position.x, gero::easing::easeInCubic)
                  .push(1.0f, k_faceOutX);
  _alphaEasing = gero::easing::GenericEasing<2>()
                   .push(0.0f, _alpha, gero::easing::easeInCubic)
                   .push(1.0f, 0.0f);

  _isVisible = false;
}

void
FitnessDataRenderer::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _position.x = _moveEasing.get(_timer.getCoefElapsed());
    _alpha = _alphaEasing.get(_timer.getCoefElapsed());
  }
}

void
FitnessDataRenderer::resize() {
  if (_isVisible)
    fadeIn(0.0f, 0.2f);
  else
    fadeOut(0.0f, 0.2f);
}

void
FitnessDataRenderer::renderWireframe() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;

  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 whiteColor(1.0f, 1.0f, 1.0f);

  stackRenderers.triangles.pushQuad(
    glm::vec2(_position + _size * 0.5f), _size, glm::vec4(0, 0, 0, 0.75f),
    -0.1f);
  stackRenderers.wireframes.pushRectangle(_position, _size, whiteColor, -0.1f);

  const float maxFitness = logic.fitnessStats.max();
  float stepWidth = _size.x / (logic.fitnessStats.size() - 1);

  for (std::size_t ii = 1; ii < logic.fitnessStats.size(); ++ii) {
    const float prevData = logic.fitnessStats.get(ii - 1);
    const float currData = logic.fitnessStats.get(ii);

    const glm::vec2 prevPos = {
      stepWidth * (ii - 1),
      (prevData / maxFitness) * _size.y,
    };
    const glm::vec2 currPos = {
      stepWidth * ii,
      (currData / maxFitness) * _size.y,
    };

    stackRenderers.wireframes.pushLine(
      glm::vec3(_position + prevPos, 0.0f),
      glm::vec3(_position + currPos, 0.0f), whiteColor);

    stackRenderers.wireframes.pushLine(
      glm::vec3(_position.x + prevPos.x, _position.y + prevPos.y, 0.0f),
      glm::vec3(_position.x + prevPos.x, _position.y, 0.0f),
      //glm::vec3(_position + currPos, 0.0f),
      whiteColor);
  }
}

void
FitnessDataRenderer::renderHudText() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& textRenderer = graphic.hud.textRenderer;
  // auto& stackRenderers = graphic.hud.stackRenderers;

  const unsigned int totalCars = logic.cores.totalCars;
  unsigned int carsLeft = 0;
  float localBestFitness = 0.0f;

  for (unsigned int ii = 0; ii < totalCars; ++ii) {
    const auto& carData = simulation.getCarResult(ii);

    if (carData.isAlive)
      ++carsLeft;

    if (localBestFitness < carData.fitness)
      localBestFitness = carData.fitness;
  }

  const float bestFitness = simulation.getBestGenome().getFitness();

  const glm::vec4 color = glm::vec4(glm::vec3(0.8f), _alpha);
  const glm::vec4 outlineColor = glm::vec4(glm::vec3(0.4f), _alpha);
  const glm::vec4 colorRed = glm::vec4(0.5f, 0.0f, 0.0f, _alpha);
  const glm::vec4 colorGreen = glm::vec4(0.0f, 0.5f, 0.0f, _alpha);
  const float textDepth = 0.25f;

  const glm::vec2 basePos = { k_textHScale, k_textHScale + 4.0f * k_textScale};
  glm::vec2 currPos = basePos;

  {
    const glm::vec2 textPos = currPos;

    std::stringstream sstr;

    sstr << "Generation: " << simulation.getGenerationNumber();

    sstr << std::endl;

    sstr << std::fixed << std::setprecision(1);
    sstr << "Fitness:" << std::endl;
    sstr << "${1}" << localBestFitness << "${0}/" << bestFitness;
    if (bestFitness > 0)
      sstr << " (${1}" << int32_t((localBestFitness / bestFitness) * 100.0f) << "%${0})";
    else
      sstr << " (${1}" << int32_t(localBestFitness * 100.0f) << "%${0})";

    sstr << std::endl;

    sstr << "Cars:" << std::endl;
    sstr << std::fixed << std::setprecision(1);
    sstr << "${2}" << carsLeft << "${0}/" << totalCars;
    sstr << " (${2}" << int32_t((float(carsLeft) / totalCars) * 100) << "%${0})";

    const std::string str = sstr.str();

    float bestFitnessCoef = 0.0f;
    if (bestFitness == 0.0f) {
      bestFitnessCoef = localBestFitness > 0.0f ? 1.0f : 0.5f;
    } else {
      bestFitnessCoef = gero::math::clamp(localBestFitness / bestFitness, 0.0f, 1.0f);
    }

    const glm::vec4 bestFitnessColor = glm::mix(colorRed, colorGreen, bestFitnessCoef);

    const float carLeftCoef = 1.0f - gero::math::clamp(float(carsLeft) / totalCars, 0.0f, 1.0f);

    const glm::vec4 carLeftColor = glm::mix(colorGreen, colorRed, carLeftCoef);

    textRenderer.pushText(
      textPos,
      str,
      color,
      k_textScale,
      textDepth,
      outlineColor,
      TextRenderer::TextAlign::left,
      //
      TextRenderer::State(color, outlineColor),
      TextRenderer::State(color, bestFitnessColor),
      TextRenderer::State(color, carLeftColor)
      );

    helpers::renderTextBackground(
      textDepth,
      glm::vec4(0.0f, 0.0f, 0.0f, _alpha),
      glm::vec4(0.3f, 0.3f, 0.3f, _alpha),
      3.0f,
      6.0f
    );

  }

}
