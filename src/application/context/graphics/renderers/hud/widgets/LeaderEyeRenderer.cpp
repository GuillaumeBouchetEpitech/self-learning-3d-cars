
#include "LeaderEyeRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/states/StateManager.hpp"

#include "geronimo/system/easing/easingFunctions.hpp"

#include <iomanip>

namespace {

constexpr float k_faceInX = -10.0f;
constexpr float k_faceOutX = +110.0f;

constexpr float k_sizeX = 100.0f;

} // namespace

void
LeaderEyeRenderer::initialize() {
  auto& context = Context::get();
  const glm::vec2 vSize = glm::vec2(context.graphic.renderer.getHudRenderer().getCamera().getSize());

  _position.x = vSize.x - k_sizeX + k_faceOutX;
  _position.y = 10;
}

void
LeaderEyeRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);

  auto& context = Context::get();
  auto& graphic = context.graphic;

  const glm::vec2 vSize = glm::vec2(graphic.renderer.getHudRenderer().getCamera().getSize());
  const float targetPos = vSize.x - k_sizeX + k_faceInX;

  _moveEasing = gero::easing::GenericEasing<2>();
  _moveEasing.push(0.0f, _position.x, gero::easing::easeOutCubic);
  _moveEasing.push(1.0f, targetPos);

  _isVisible = true;
}

void
LeaderEyeRenderer::fadeOut(float delay, float duration) {
  _timer.start(delay, duration);

  auto& context = Context::get();
  auto& graphic = context.graphic;

  const glm::vec2 vSize = glm::vec2(graphic.renderer.getHudRenderer().getCamera().getSize());
  const float targetPos = vSize.x - k_sizeX + k_faceOutX;

  _moveEasing = gero::easing::GenericEasing<2>();
  _moveEasing.push(0.0f, _position.x, gero::easing::easeInCubic);
  _moveEasing.push(1.0f, targetPos);

  _isVisible = false;
}

void
LeaderEyeRenderer::resize() {
  if (_isVisible)
    fadeIn(0.0f, 0.1f);
  else
    fadeOut(0.0f, 0.2f);
}

void
LeaderEyeRenderer::update(float elapsedTime) {

  auto& context = Context::get();
  auto& logic = context.logic;

  const StateManager::States currentState = StateManager::get()->getState();
  const bool validSate = (currentState == StateManager::States::Running);

  if (!_isVisible && validSate && logic.leaderCar.hasLeader()) {
    fadeIn(0.5f, 0.5f);
    _isVisible = true;
  } else if (_isVisible && (!validSate || !logic.leaderCar.hasLeader())) {
    fadeOut(0.0f, 0.5f);
    _isVisible = false;
  }

  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _position.x = _moveEasing.get(_timer.getCoefElapsed());
  }
}

void
LeaderEyeRenderer::render() {
  auto& context = Context::get();
  auto& logic = context.logic;

  if (!logic.leaderCar.hasLeader())
    return;

  auto& stackRenderers = context.graphic.renderer.getHudRenderer().getStackRenderers();

  auto leaderData = logic.leaderCar.leaderData();

  const glm::vec2 size = {k_sizeX, 60};
  const glm::vec2 position = {_position.x, 305};

  const glm::vec4 whiteColor(0.8f, 0.8f, 0.8f, 1.0f);
  const glm::vec4 greenColor(0.0f, 0.8f, 0.0f, 1.0f);
  const glm::vec4 redColor(0.8f, 0.0f, 0.0f, 1.0f);

  //
  //

  const unsigned int totalInputs = context.logic.annTopology.getInputLayerSize();
  const unsigned int layerSize = 5; // <= hardcoded :(
  const unsigned int layerCount = totalInputs / layerSize;

  //
  //

  std::vector<glm::vec2> allPositions;
  allPositions.reserve(totalInputs); // pre-allocate
  for (unsigned int ii = 0; ii < layerCount; ++ii)
    for (unsigned int jj = 0; jj < layerSize; ++jj) {
      glm::vec2 currPos = position;

      currPos.y += size.y;

      currPos.x += size.x * ((float(jj) + 0.5f) / layerSize);
      currPos.y -= size.y * ((float(ii) + 0.5f) / layerCount);

      allPositions.push_back(currPos);
    }

  glm::vec2 eyeSize = {20, 20};

  for (std::size_t ii = 0; ii < allPositions.size(); ++ii) {
    const auto& position = allPositions.at(ii);
    const float val = leaderData->eyeSensors.at(ii).value;

    const glm::vec4 color = glm::mix(redColor, greenColor, val);

    stackRenderers.getTrianglesStack().pushQuad(position, eyeSize, color, 0.4f);
  }

  for (const auto& position : allPositions) {
    stackRenderers.getWireFramesStack().pushRectangle(position - eyeSize * 0.5f, eyeSize, whiteColor, 0.5f);
  }

  stackRenderers.getWireFramesStack().pushRectangle(position, size, whiteColor, 0.5f);
}
