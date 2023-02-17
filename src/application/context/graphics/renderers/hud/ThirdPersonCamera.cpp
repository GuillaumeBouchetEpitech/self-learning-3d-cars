
#include "ThirdPersonCamera.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"

#include <iomanip>
#include <sstream>

namespace {

constexpr float k_faceInX = -10.0f;
constexpr float k_faceOutX = +500.0f;

} // namespace

void
ThirdPersonCamera::initialise() {
  auto& context = Context::get();
  const auto& vSize = context.graphic.cameraData.viewportSize;

  _size = {175, 150};

  _position.x = vSize.x - _size.x + k_faceOutX;
  _position.y = 10;

  _postProcess.initialise({_size.x, _size.y});
}

bool
ThirdPersonCamera::canActivate() const {
  auto& context = Context::get();
  auto& logic = context.logic;
  const auto& leaderCar = logic.leaderCar;

  // do not update the third person gero::graphics::Camera if not in a correct
  // state
  const StateManager::States currentState = StateManager::get()->getState();
  if (
    currentState == StateManager::States::Running ||
    currentState == StateManager::States::StartGeneration) {

    // valid leading car?
    if (logic.isAccelerated || !leaderCar.hasLeader())
      return false;
  }

  return true;
}

void
ThirdPersonCamera::fadeIn(float delay, float duration) {

  auto& context = Context::get();
  auto& graphic = context.graphic;

  const auto& vSize = graphic.cameraData.viewportSize;
  const float targetPos = vSize.x - _size.x + k_faceInX;

  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>()
                  .push(0.0f, _position.x, gero::easing::easeOutCubic)
                  .push(1.0f, targetPos);

  _isVisible = true;
}

void
ThirdPersonCamera::fadeOut(float delay, float duration) {

  auto& context = Context::get();
  auto& graphic = context.graphic;

  const auto& vSize = graphic.cameraData.viewportSize;
  const float targetPos = vSize.x - _size.x + k_faceOutX;

  _timer.start(delay, duration);

  _moveEasing = gero::easing::GenericEasing<2>()
                  .push(0.0f, _position.x, gero::easing::easeInCubic)
                  .push(1.0f, targetPos);

  _isVisible = false;
}

void
ThirdPersonCamera::update(float elapsedTime) {
  if (!_timer.isDone()) {
    _timer.update(elapsedTime);
    _position.x = _moveEasing.get(_timer.getCoefElapsed());
  }

  if (canActivate()) {
    auto& context = Context::get();
    auto& leaderCar = context.logic.leaderCar;

    if (auto leaderData = leaderCar.leaderData()) {
      const auto& chassis = leaderData->liveTransforms.chassis;
      const glm::vec3 carOrigin = chassis.position + glm::mat3_cast(chassis.orientation) * glm::vec3(0.0f, 0.0f, 2.5f);
      const glm::vec3 carUpAxis = glm::mat3_cast(chassis.orientation) * glm::vec3(0.0f, 0.0f, 1.0f);

      if (
        // do not update the third person gero::graphics::Camera if too close
        // from the target
        glm::distance(carOrigin, _eye) > 0.25f) {
        // simple lerp to setup the third person gero::graphics::Camera
        const float lerpRatio = 0.1f * 60.0f * elapsedTime;
        _eye += (carOrigin - _eye) * lerpRatio;
        _upAxis += (carUpAxis - _upAxis) * lerpRatio;
      }

      const glm::vec3 eye = _eye;
      const glm::vec3 target = carOrigin;
      const glm::vec3 upAxis = _upAxis;

      _camera.setSize(_size.x, _size.y);
      _camera.setPerspective(70.0f, 0.1f, 1500.0f);
      _camera.lookAt(eye, target, upAxis);
      _camera.computeMatrices();
    }
  }
}

void
ThirdPersonCamera::resize() {
  if (_isVisible)
    fadeIn(0.0f, 0.2f);
  else
    fadeOut(0.0f, 0.2f);
}

void
ThirdPersonCamera::render() {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& cameraData = graphic.cameraData;

  if (canActivate()) {
    const auto& vSize = cameraData.viewportSize;

    _postProcess.startRecording();

    Scene::renderScene(_camera);

    _postProcess.stopRecording();

    gero::graphics::GlContext::setViewport(0, 0, vSize.x, vSize.y);
  }

  {
    const auto& matricesData = cameraData.hud.getMatricesData();
    _postProcess.setMatricesData(matricesData);

    _postProcess.setGeometry(_position, _size, -1.0f);

    _postProcess.render();

    auto& stackRenderers = graphic.hud.stackRenderers;
    stackRenderers.triangles.pushQuad(
      _position + _size * 0.5f, _size, glm::vec4(0, 0, 0, 0.75f), -1.5f);
    stackRenderers.wireFrames.pushRectangle(
      _position, _size, glm::vec4(0.8f, 0.8f, 0.8f, 1), -0.1f);
  }
}
