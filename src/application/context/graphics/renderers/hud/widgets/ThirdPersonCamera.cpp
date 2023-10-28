
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
ThirdPersonCamera::initialize() {
  auto& context = Context::get();
  const glm::vec2 vSize = glm::vec2(context.graphic.renderer.getHudRenderer().getCamera().getSize());

  _layout.size = {175, 150};

  _layout.position.x = vSize.x - _layout.size.x + k_faceOutX;
  _layout.position.y = 10;

  _postProcess.initialize(_layout.size.x, _layout.size.y);
}

bool
ThirdPersonCamera::canActivate() const {
  auto& context = Context::get();
  auto& logic = context.logic;
  const auto& leaderCar = logic.leaderCar;

  // do not update the third person gero::graphics::Camera if not in a correct
  // state
  const StateManager::States currentState = StateManager::get()->getState();
  if (currentState == StateManager::States::Running || currentState == StateManager::States::StartGeneration) {

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

  const glm::vec2 vSize = glm::vec2(graphic.renderer.getHudRenderer().getCamera().getSize());
  const float targetPos = vSize.x - _layout.size.x + k_faceInX;

  _layout.timer.start(delay, duration);

  _layout.moveEasing = gero::easing::GenericEasing<2>();
  _layout.moveEasing.push(0.0f, _layout.position.x, gero::easing::easeOutCubic);
  _layout.moveEasing.push(1.0f, targetPos);

  _layout.isVisible = true;
}

void
ThirdPersonCamera::fadeOut(float delay, float duration) {

  auto& context = Context::get();
  auto& graphic = context.graphic;

  const glm::vec2 vSize = glm::vec2(graphic.renderer.getHudRenderer().getCamera().getSize());
  const float targetPos = vSize.x - _layout.size.x + k_faceOutX;

  _layout.timer.start(delay, duration);

  _layout.moveEasing = gero::easing::GenericEasing<2>();
  _layout.moveEasing.push(0.0f, _layout.position.x, gero::easing::easeInCubic);
  _layout.moveEasing.push(1.0f, targetPos);

  _layout.isVisible = false;
}

void
ThirdPersonCamera::update(float elapsedTime) {
  if (!_layout.timer.isDone()) {
    _layout.timer.update(elapsedTime);
    _layout.position.x = _layout.moveEasing.get(_layout.timer.getCoefElapsed());
  }

  if (canActivate()) {
    auto& context = Context::get();
    auto& leaderCar = context.logic.leaderCar;

    if (auto leaderData = leaderCar.leaderData()) {
      const auto& chassis = leaderData->liveTransforms.chassis;

      if (leaderData->isDying) {

        // simple lerp to setup the third person gero::graphics::Camera
        const float lerpRatio = 0.3f * 60.0f * elapsedTime;

        const float k_desiredDistance = 6.0f;

        const glm::vec3 desiredTarget = chassis.position;
        // const glm::vec3 desiredUpAxis = glm::vec3(0.0f, 0.0f, 1.0f);
        // const glm::vec3 desiredEye = chassis.position +
        // glm::vec3(-k_desiredDistance * 2.0f, -k_desiredDistance * 2.0f,
        // k_desiredDistance * 1.25f); const glm::vec3 desiredEye =
        // chassis.position + glm::vec3(-k_desiredDistance * 2.0f,
        // -k_desiredDistance * 2.0f, k_desiredDistance * 1.25f);
        const glm::vec3 desiredEye = _eye + (desiredTarget - _target);

        const float currDistance = glm::distance(desiredTarget, _eye);

        float mode = 0.0f;
        if (currDistance > k_desiredDistance + 0.20f) {
          mode = 1.0f;
        } else if (currDistance < k_desiredDistance - 0.20f) {
          mode = -1.0f;
        }

        _eye += (desiredEye - _eye) * lerpRatio * mode;
        _target += (desiredTarget - _target) * lerpRatio;
        // _upAxis += (desiredUpAxis - _upAxis) * lerpRatio;

        _camera.setSize(_layout.size.x, _layout.size.y);
        _camera.setPerspective(70.0f, 0.1f, 1500.0f);
        _camera.lookAt(_eye, _target, _upAxis);
        _camera.computeMatrices();

      } else {
        const glm::mat3 orientation = glm::mat3_cast(chassis.orientation);

        // simple lerp to setup the third person gero::graphics::Camera
        const float lerpRatio = 0.1f * 60.0f * elapsedTime;

        const float k_desiredDistance = 2.75f;

        const glm::vec3 desiredTarget = chassis.position + orientation * glm::vec3(0.0f, 0.0f, 2.5f);
        const glm::vec3 desiredUpAxis = orientation * glm::vec3(0.0f, 0.0f, 1.0f);
        const glm::vec3 desiredEye =
          chassis.position + orientation * glm::vec3(0.0f, -k_desiredDistance * 2.0f, k_desiredDistance * 1.25f);

        const float currDistance = glm::distance(desiredTarget, _eye);

        float mode = 0.0f;
        if (currDistance > k_desiredDistance + 0.20f) {
          mode = 1.0f;
        } else if (currDistance < k_desiredDistance - 0.20f) {
          mode = -1.0f;
        }

        _eye += (desiredEye - _eye) * lerpRatio * mode;
        _target += (desiredTarget - _target) * lerpRatio;
        _upAxis += (desiredUpAxis - _upAxis) * lerpRatio;

        _camera.setSize(_layout.size.x, _layout.size.y);
        _camera.setPerspective(70.0f, 0.1f, 1500.0f);
        _camera.lookAt(_eye, _target, _upAxis);
        _camera.computeMatrices();
      }
    }
  }
}

void
ThirdPersonCamera::resize() {
  if (_layout.isVisible)
    fadeIn(0.0f, 0.2f);
  else
    fadeOut(0.0f, 0.2f);
}

void
ThirdPersonCamera::render() {

  auto& context = Context::get();
  auto& renderer = context.graphic.renderer;
  auto& stackRenderers = renderer.getHudRenderer().getStackRenderers();
  const auto& hudCamera = renderer.getHudRenderer().getCamera();

  if (canActivate()) {
    const auto& vSize = hudCamera.getSize();

    _postProcess.startRecording();

    Scene::renderScene(_camera);

    _postProcess.stopRecording();

    gero::graphics::GlContext::setViewport(0, 0, vSize.x, vSize.y);
  }

  {
    const auto& matricesData = hudCamera.getMatricesData();
    _postProcess.setMatricesData(matricesData);

    _postProcess.setGeometry(_layout.position, _layout.size, -1.0f);

    _postProcess.render();

    stackRenderers.getTrianglesStack().pushQuad(
      _layout.position + _layout.size * 0.5f, _layout.size, glm::vec4(0, 0, 0, 0.75f), -1.5f);
    stackRenderers.getWireFramesStack().pushRectangle(
      _layout.position, _layout.size, glm::vec4(0.8f, 0.8f, 0.8f, 1), -0.1f);
  }
}
