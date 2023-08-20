
#include "application/defines.hpp"

#include "StateManager.hpp"
#include "State_AbstractSimulation.hpp"

#include "application/context/helpers/inputManagers/KeyboardManager.hpp"
#include "application/context/helpers/inputManagers/MouseManager.hpp"
#include "application/context/helpers/inputManagers/TouchManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"

#include <cmath>  // std::ceil
#include <limits> // std::numeric_limits<T>::max();

void
State_AbstractSimulation::enter() {
  // D_MYLOG("step");
}

void
State_AbstractSimulation::leave() {
  // D_MYLOG("step");
}

//

void
State_AbstractSimulation::handleEvent(const SDL_Event& event) {
  auto& context = Context::get();
  auto& keyboard = KeyboardManager::get();
  auto& mouse = MouseManager::get();
  auto& touch = TouchManager::get();

  switch (event.type) {
  case SDL_KEYDOWN: {
    keyboard.updateAsPressed(event.key.keysym.sym);
    break;
  }
  case SDL_KEYUP: {
    keyboard.updateAsReleased(event.key.keysym.sym);
    break;
  }

  case SDL_MOUSEBUTTONDOWN: {
    mouse.updateAsPressed(event.button.button);
    if (mouse.setLock(true))
      mouse.resetDelta();
    break;
  }
  case SDL_MOUSEBUTTONUP: {
    mouse.setLock(false);
    mouse.updateAsReleased(event.button.button);
    break;
  }
  case SDL_MOUSEMOTION: {

    float deltaX = float(event.motion.xrel);
    float deltaY = float(event.motion.yrel);

#ifdef D_WEB_BUILD
    deltaX *= 0.5f;
    deltaY *= 0.5f;
#endif

    mouse.updateDelta(deltaX, deltaY);
    break;
  }

  case SDL_FINGERDOWN: {
    const glm::vec2 vSize = glm::vec2(context.graphic.cameraData.viewportSize);
    const glm::vec2 currPos = glm::vec2(event.tfinger.x, event.tfinger.y) * vSize;
    touch.updateAsTouchedDown(int32_t(event.tfinger.fingerId), currPos);
    break;
  }
  case SDL_FINGERUP: {
    touch.updateAsTouchedUp(int32_t(event.tfinger.fingerId));
    break;
  }
  case SDL_FINGERMOTION: {
    const glm::vec2 vSize = glm::vec2(context.graphic.cameraData.viewportSize);
    const glm::vec2 currPos = glm::vec2(event.tfinger.x, event.tfinger.y) * vSize;
    const glm::vec2 currDelta = glm::vec2(event.tfinger.dx, event.tfinger.dy) * vSize;
    touch.updateAsTouchedMotion(int32_t(event.tfinger.fingerId), currPos, currDelta);
    break;
  }

  default:
    break;
  }
}

void
State_AbstractSimulation::update(float elapsedTime) {

  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& cameraData = graphic.cameraData;

  { // events

    auto& rotations = cameraData.rotations;

    const bool hasTouchEvent = TouchManager::get().getTouchData(0).has_value();
    const bool hasMouseEvent = MouseManager::get().isLocked();

    if (hasTouchEvent) {

      // touch OR mouse, not both

      if (auto touch = TouchManager::get().getTouchData(0)) {
        rotations.theta -= float(touch->get().delta.x) * 1.0f * elapsedTime;
        rotations.phi += float(touch->get().delta.y) * 1.0f * elapsedTime;
      }

    } else if (hasMouseEvent) {

      // mouse/touch event(s)

      auto& mouse = MouseManager::get();

      if (mouse.isLocked()) {
        rotations.theta -= float(mouse.getDelta().x) * 1.0f * elapsedTime;
        rotations.phi += float(mouse.getDelta().y) * 1.0f * elapsedTime;
      }
      mouse.resetDelta();

    } // mouse/touch event(s)

    { // keyboard event(s)

      auto& keyboard = KeyboardManager::get();

      const bool rotateUp = keyboard.isPressed(SDLK_UP, SDLK_w, SDLK_z);
      const bool rotateDown = keyboard.isPressed(SDLK_DOWN, SDLK_s);
      const bool rotateLeft = keyboard.isPressed(SDLK_LEFT, SDLK_q, SDLK_a);
      const bool rotateRight = keyboard.isPressed(SDLK_RIGHT, SDLK_d);
      const bool accelerate = keyboard.isPressed(SDLK_SPACE);

      if (rotateLeft)
        rotations.theta += 2.0f * elapsedTime;
      else if (rotateRight)
        rotations.theta -= 2.0f * elapsedTime;

      if (rotateUp)
        rotations.phi -= 1.0f * elapsedTime;
      else if (rotateDown)
        rotations.phi += 1.0f * elapsedTime;

      context.logic.isAccelerated = accelerate;

    } // keyboard event(s)

  } // events
}

void
State_AbstractSimulation::render(const SDL_Window& window) {
  static_cast<void>(window); // <= unused

  Scene::renderAll();
}

void
State_AbstractSimulation::resize(int width, int height) {
  auto& graphic = Context::get().graphic;

  const glm::vec2 newSize(width, height);

  graphic.cameraData.viewportSize = newSize;

  graphic.hud.postProcess.resize({width, height});
  graphic.hud.postProcess.setGeometry(glm::vec2(0, 0), newSize, -2.0f);

  graphic.hud.widgets.topologyRenderer.resize();
  graphic.hud.widgets.thirdPersonCamera.resize();
  graphic.hud.widgets.coreUsageRenderer.resize();
  graphic.hud.widgets.fitnessDataRenderer.resize();
  graphic.hud.widgets.leaderEyeRenderer.resize();
}

void
State_AbstractSimulation::visibility(bool visible) {
#if 0 // disable pause state?
    static_cast<void>(visible); // unused
#else
  auto* stateManager = StateManager::get();
  StateManager::States currentState = stateManager->getState();

  if (currentState != StateManager::States::Paused && !visible)
    stateManager->changeState(StateManager::States::Paused);
#endif
}

void
State_AbstractSimulation::_updateCommonLogic(float elapsedTime) {
  auto& graphic = Context::get().graphic;

  graphic.scene.particleManager.update(elapsedTime);
  graphic.scene.backGroundTorusRenderer.update(elapsedTime);
  graphic.scene.flockingManager->update(elapsedTime);

  graphic.hud.widgets.screenTitles.update(elapsedTime);
  graphic.hud.widgets.topologyRenderer.update(elapsedTime);
  graphic.hud.widgets.thirdPersonCamera.update(elapsedTime);
  graphic.hud.widgets.coreUsageRenderer.update(elapsedTime);
  graphic.hud.widgets.fitnessDataRenderer.update(elapsedTime);
  graphic.hud.widgets.informationTextRenderer.update(elapsedTime);
  graphic.hud.widgets.leaderEyeRenderer.update(elapsedTime);
}

void
State_AbstractSimulation::_updateCameraTracking(float elapsedTime) {
  auto& context = Context::get();
  auto& cameraData = context.graphic.cameraData;
  auto& logic = context.logic;

  glm::vec3 cameraNextCenter = logic.circuitDimension.center;
  float cameraNextDistance = 300.0f;

  //
  //

  auto& leaderCar = logic.leaderCar;
  auto& simulation = *logic.simulation;

  if (simulation.isGenerationComplete())
    return;

  if (logic.isAccelerated) {
    leaderCar.reset();
    cameraNextDistance = 200.0f;
  } else {
    leaderCar.update(elapsedTime);

    if (auto leaderPos = leaderCar.leaderPosition()) {
      cameraNextCenter = *leaderPos;
      cameraNextDistance = 40.0f;
    } else {
      cameraNextCenter = simulation.getStartPosition();
      cameraNextDistance = 60.0f;
    }
  }

  //
  //

  {
    constexpr float k_maxDistance = 200.0f;
    const float distanceToTarget = glm::distance(cameraNextCenter, cameraData.center);
    const float moveLerpRatio = gero::easing::GenericEasing<2>()
                                  .push(0.0f, 3.0f, gero::easing::easeInOutCubic)
                                  .push(1.0f, 1.0f)
                                  .get(distanceToTarget / k_maxDistance) *
                                elapsedTime;

    cameraData.center += (cameraNextCenter - cameraData.center) * moveLerpRatio;
    cameraData.distance += (cameraNextDistance - cameraData.distance) * 1.0f * elapsedTime;
  }
}
