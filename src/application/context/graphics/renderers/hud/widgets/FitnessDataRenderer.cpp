
#include "FitnessDataRenderer.hpp"

#include "application/context/Context.hpp"

#include "application/context/graphics/renderers/hud/helpers/renderTextBackground.hpp"
#include "application/context/graphics/renderers/hud/helpers/renderProgressBar.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/math/BSpline.hpp"

#include <iomanip>

namespace {

constexpr float k_faceInX = +8.0f;
constexpr float k_faceOutX = -400.0f;

constexpr float k_textScale = 16.0f;
constexpr float k_textHScale = k_textScale * 0.5f;

constexpr float k_width = 150.0f;
constexpr float k_height = 75.0f;

} // namespace

FitnessDataRenderer::FitnessDataRenderer() {
  _position.x = k_faceOutX;
  _position.y = 60 + 0.0f * k_textScale + k_textHScale;

  _size = { k_width, k_height };
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
FitnessDataRenderer::renderWireFrame() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;

  auto& stackRenderers = graphic.hud.stackRenderers;

  const glm::vec3 whiteColor(1.0f);
  const glm::vec3 lightGrayColor(0.5f);
  const glm::vec3 redColor(1.0f, 0.0f, 0.0f);
  const glm::vec3 greenColor(0.0f, 1.0f, 0.0f);



  const uint32_t totalCars = logic.cores.totalGenomes;
  float localBestFitness = 0.0f;
  for (uint32_t ii = 0; ii < totalCars; ++ii) {
    const auto& carData = logic.simulation->getCarResult(ii);
    localBestFitness = std::max(localBestFitness, carData.fitness);
  }


  const float maxFitness = std::max(logic.fitnessStats.max(), localBestFitness);

  {
    stackRenderers.getTrianglesStack().pushQuad(
      glm::vec2(_position + _size * 0.5f),
      _size,
      glm::vec4(0, 0, 0, 0.75f),
      -0.5f);

    stackRenderers.getWireFramesStack().pushRectangle(_position, _size, whiteColor, 0.5f);
  }

  {

    // const float stepWidth = _size.x / (logic.fitnessStats.size() - 1);
    const float stepWidth = _size.x / (logic.fitnessStats.size());

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

      // horizontal data curve
      stackRenderers.getWireFramesStack().pushLine(
        glm::vec3(_position + prevPos, 0.0f),
        glm::vec3(_position + currPos, 0.0f), whiteColor * 0.6f);

      // vertical delimiter
      stackRenderers.getWireFramesStack().pushLine(
        glm::vec3(_position.x + prevPos.x, _position.y + prevPos.y, 0.0f),
        glm::vec3(_position.x + prevPos.x, _position.y, 0.0f),
        lightGrayColor);
    }

    {

      const std::size_t index = logic.fitnessStats.size() - 1;

      const float prevData = logic.fitnessStats.get(index);
      const float currData = localBestFitness;

      const glm::vec2 prevPos = {
        stepWidth * (index),
        (prevData / maxFitness) * _size.y,
      };
      const glm::vec2 currPos = {
        stepWidth * (index + 1),
        (currData / maxFitness) * _size.y,
      };

      const glm::vec3& currColor = localBestFitness < maxFitness ? redColor : greenColor;

      // horizontal data curve
      stackRenderers.getWireFramesStack().pushLine(
        glm::vec3(_position + prevPos, 0.0f),
        glm::vec3(_position + currPos, 0.0f),
        currColor);

      // vertical delimiter
      stackRenderers.getWireFramesStack().pushLine(
        glm::vec3(_position.x + prevPos.x, _position.y + prevPos.y, 0.0f),
        glm::vec3(_position.x + prevPos.x, _position.y, 0.0f),
        lightGrayColor);
    }

  }

  //
  //
  //

  {

    struct Dividers
    {
      float step;
      float limit;
      glm::vec3 color;
      float zValue;
    };

    const std::array<Dividers, 4> allDividers =
    {{
      { 1.0f, 10.0f, glm::vec3(0.4f, 0.4f, 0.4f), -0.4f },
      { 2.5f, 40.0f, glm::vec3(0.5f, 0.5f, 0.0f), -0.3f },
      { 5.0f, 100.0f, glm::vec3(0.75f, 0.25f, 0.25f), -0.2f },
      { 10.0f, 999999.0f, glm::vec3(0.25f, 0.75f, 0.25f), -0.1f },
    }};

    for (const auto& values : allDividers)
    {

      if (maxFitness < values.step || maxFitness > values.limit)
        continue;

      const uint32_t totalSteps = uint32_t(maxFitness / values.step);

      const float stepHeight = _size.y / maxFitness * values.step;

      // D_MYLOG("stepHeight " << stepHeight);

      const float x1 = _position.x;
      const float x2 = x1 + _size.x;

      for (uint32_t ii = 0; ii < totalSteps; ++ii)
      {
        const float mainY = _position.y + float(ii + 1) * stepHeight;

        stackRenderers.getWireFramesStack().pushLine(
          glm::vec3(x1, mainY, values.zValue),
          glm::vec3(x2, mainY, values.zValue),
          values.color
        );
      }

    }

  }

  //
  //
  //

  // {

  //   gero::math::BSpline bsplineSmoother;

  //   gero::math::BSpline::Definition smootherDef;
  //   smootherDef.degree = 4;
  //   smootherDef.dimensions = 1;
  //   smootherDef.knotsLength = logic.fitnessStats.size() * smootherDef.dimensions;
  //   smootherDef.getDataCallback = [&logic](uint32_t index)
  //   {
  //     return logic.fitnessStats.get(index);
  //   };
  //   bsplineSmoother.initialize(smootherDef);

  //   constexpr unsigned int maxIterations = 100;
  //   constexpr float k_step = 1.0f / maxIterations; // tiny steps

  //   float prevCoef = k_step * 1.0f;
  //   float prevDelta = bsplineSmoother.calcAt(prevCoef, 0);

  //   for (float currCoef = k_step * 2.0f; currCoef < 1.0f; currCoef += k_step) {

  //     const float currDelta = bsplineSmoother.calcAt(currCoef, 0);

  //     const float prevHeight = _size.y * prevDelta / maxFitness;
  //     const float currHeight = _size.y * currDelta / maxFitness;

  //     stackRenderers.getWireFramesStack().pushLine(
  //       glm::vec3(_position, 0.0f) + glm::vec3(prevCoef * _size.x, prevHeight, 0.1f),
  //       glm::vec3(_position, 0.0f) + glm::vec3(currCoef * _size.x, currHeight, 0.1f),
  //       whiteColor);

  //     prevCoef = currCoef;
  //     prevDelta = currDelta;
  //   }

  // }




}

void
FitnessDataRenderer::renderHudText() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& textRenderer = graphic.hud.textRenderer;

  const uint32_t totalCars = logic.cores.totalGenomes;
  float localBestFitness = 0.0f;
  for (uint32_t ii = 0; ii < totalCars; ++ii) {
    const auto& carData = simulation.getCarResult(ii);
    localBestFitness = std::max(localBestFitness, carData.fitness);
  }

  const uint32_t liveAgents = simulation.getLiveGenomes();
  const uint32_t agentsLeft = simulation.getWaitingGenomes() + liveAgents;


  const float bestFitness = simulation.getBestGenome().getFitness();

  const glm::vec4 textColor = glm::vec4(glm::vec3(0.8f), _alpha);
  const glm::vec4 textOutlineColor = glm::vec4(glm::vec3(0.4f), _alpha);
  const glm::vec4 colorRed = glm::vec4(0.5f, 0.0f, 0.0f, _alpha);
  const glm::vec4 colorGreen = glm::vec4(0.0f, 0.5f, 0.0f, _alpha);
  constexpr float k_textDepth = 0.25f;

  const glm::vec2 basePos = { _position.x, _position.y - 1.5f * k_textScale};

  glm::vec2 currPos = basePos;

  {
    const glm::vec2 textPos = currPos;

    std::stringstream sstr;

    sstr << "Generation: " << simulation.getGenerationNumber();

    sstr << std::endl;

    sstr << std::fixed << std::setprecision(1);
    sstr << "Fitness:" << std::endl;
    sstr << "${1}" << localBestFitness << "${0}/" << bestFitness;

    const std::string str = sstr.str();

    float bestFitnessCoef = 0.0f;
    if (bestFitness == 0.0f) {
      bestFitnessCoef = localBestFitness > 0.0f ? 1.0f : 0.5f;
    } else {
      bestFitnessCoef = gero::math::clamp(localBestFitness / bestFitness, 0.0f, 1.0f);
    }

    const glm::vec4 bestFitnessColor = glm::mix(colorRed, colorGreen, bestFitnessCoef);

    const float carLeftCoef = 1.0f - gero::math::clamp(float(agentsLeft) / totalCars, 0.0f, 1.0f);

    const glm::vec4 carLeftColor = glm::mix(colorGreen, colorRed, carLeftCoef);

    textRenderer.setMainColor(textColor);
    textRenderer.setOutlineColor(textOutlineColor);
    textRenderer.setScale(k_textScale);
    textRenderer.setDepth(k_textDepth);
    textRenderer.setTextAlign(gero::graphics::TextRenderer::TextAlign::left);

    textRenderer.pushText(
      textPos,
      str,
      //
      gero::graphics::TextRenderer::State(textColor, textOutlineColor),
      gero::graphics::TextRenderer::State(textColor, bestFitnessColor),
      gero::graphics::TextRenderer::State(textColor, carLeftColor)
      );

    helpers::renderTextBackground(
      k_textDepth,
      glm::vec4(0.0f, 0.0f, 0.0f, _alpha),
      glm::vec4(0.3f, 0.3f, 0.3f, _alpha),
      3.0f,
      6.0f
    );

  }

}
