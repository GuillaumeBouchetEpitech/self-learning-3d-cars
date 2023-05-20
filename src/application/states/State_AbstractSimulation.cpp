
#include "application/defines.hpp"

#include "StateManager.hpp"
#include "State_AbstractSimulation.hpp"

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
  auto& keys = context.inputs.keys;
  auto& mouse = context.inputs.mouse;

  switch (event.type) {
  case SDL_KEYDOWN: {
    keys[event.key.keysym.sym] = true;
    break;
  }
  case SDL_KEYUP: {
    keys[event.key.keysym.sym] = false;
    break;
  }

  case SDL_MOUSEBUTTONDOWN: {
    mouse.position = {event.motion.x, event.motion.y};
    mouse.delta = {0, 0};
    mouse.tracking = true;
    break;
  }
  case SDL_MOUSEBUTTONUP: {
    mouse.tracking = false;
    break;
  }
  case SDL_MOUSEMOTION: {
    if (mouse.tracking) {
      // mouse/touch events are 4 times more sentitive in web build
      // this bit ensure the mouse/touch are even in native/web build
#if defined D_WEB_BUILD
      constexpr float coef = 1;
#else
      constexpr float coef = 4;
#endif

      const glm::vec2 newPosition = {event.motion.x, event.motion.y};
      mouse.delta = (newPosition - glm::vec2(mouse.position)) * coef;
      mouse.position = newPosition;
    }
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

    { // mouse/touch event(s)

      auto& mouse = context.inputs.mouse;

      if (mouse.tracking) {
        rotations.theta -= float(mouse.delta.x) * 1.0f * elapsedTime;
        rotations.phi += float(mouse.delta.y) * 1.0f * elapsedTime;
        mouse.delta = {0, 0};
      }

    } // mouse/touch event(s)

    { // keyboard event(s)

      auto& keys = context.inputs.keys;

      bool rotateLeft =
        (keys[SDLK_LEFT] || // ARROW
         keys[SDLK_q] ||    // QWERTY (UK-US keyboard layout)
         keys[SDLK_a]       // AZERTY (FR keyboard layout)
        );

      bool rotateRight =
        (keys[SDLK_RIGHT] || // ARROW
         keys[SDLK_d]);

      bool rotateUp =
        (keys[SDLK_UP] || // ARROW
         keys[SDLK_w] ||  // QWERTY (UK-US keyboard layout)
         keys[SDLK_z]     // AZERTY (FR keyboard layout)
        );

      bool rotateDown =
        (keys[SDLK_DOWN] || // ARROW
         keys[SDLK_s]);

      if (rotateLeft)
        rotations.theta += 2.0f * elapsedTime;
      else if (rotateRight)
        rotations.theta -= 2.0f * elapsedTime;

      if (rotateUp)
        rotations.phi -= 1.0f * elapsedTime;
      else if (rotateDown)
        rotations.phi += 1.0f * elapsedTime;

      context.logic.isAccelerated = (keys[SDLK_SPACE]); // spacebar

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
  graphic.scene.flockingManager.update(elapsedTime);

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
