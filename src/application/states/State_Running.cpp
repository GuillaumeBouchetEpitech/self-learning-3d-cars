
#include "application/defines.hpp"

#include "StateManager.hpp"
#include "State_Running.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <cmath>  // std::ceil
#include <limits> // std::numeric_limits<T>::max();

void
State_Running::enter() {
  auto& hud = Context::get().graphic.hud;
  hud.widgets.topologyRenderer.fadeIn(0.5f, 0.5f);
  hud.widgets.thirdPersonCamera.fadeIn(0.6f, 0.5f);
  hud.widgets.coreUsageRenderer.fadeIn(0.7f, 0.5f);
  hud.widgets.fitnessDataRenderer.fadeIn(0.8f, 0.5f);
  hud.widgets.informationTextRenderer.fadeIn(0.9f, 0.5f);
}

void
State_Running::leave() {
  auto& hud = Context::get().graphic.hud;
  hud.widgets.topologyRenderer.fadeOut(0.0f, 0.5f);
  hud.widgets.thirdPersonCamera.fadeOut(0.1f, 0.5f);
  hud.widgets.coreUsageRenderer.fadeOut(0.2f, 0.5f);
  hud.widgets.fitnessDataRenderer.fadeOut(0.3f, 0.5f);
  hud.widgets.informationTextRenderer.fadeOut(0.3f, 0.5f);
}

void
State_Running::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  auto& context = Context::get();
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& graphic = context.graphic;

  { // simulation update


    if (logic.carDataFrameHandler.needNewFrame())
    {

      if (simulation.isGenerationComplete() == false) {
        constexpr float fakeElapsedTime = 1.0f / 50.0f; // TODO: hardcoded
        const unsigned int totalSteps = (logic.isAccelerated ? 50 : 1);

        simulation.update(fakeElapsedTime, totalSteps);
      } else {
        simulation.breed();
      }

    }

    logic.carDataFrameHandler.update(elapsedTime);

  } // simulation update

  // done to avoid a spurious change of gero::graphics::Camera
  // -> true when changing states: Running -> EndGeneration
  if (StateManager::get()->getState() == StateManager::States::Running) {

    graphic.scene.modelsRenderer.update(elapsedTime);

    _updateCameraTracking(elapsedTime);
    _updateCommonLogic(elapsedTime);

    if (!simulation.isGenerationComplete()) {
      graphic.scene.animatedCircuitRenderer.update(elapsedTime);
    }
  }
}
