
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
  auto& widgets = Context::get().graphic.renderer.getHudRenderer().getWidgets();
  widgets.topologyRenderer.fadeIn(0.5f, 0.5f);
  widgets.thirdPersonCamera.fadeIn(0.6f, 0.5f);
  widgets.coreUsageRenderer.fadeIn(0.7f, 0.5f);
  widgets.fitnessDataRenderer.fadeIn(0.8f, 0.5f);
  widgets.informationTextRenderer.fadeIn(0.9f, 0.5f);
}

void
State_Running::leave() {
  auto& widgets = Context::get().graphic.renderer.getHudRenderer().getWidgets();
  widgets.topologyRenderer.fadeOut(0.0f, 0.5f);
  widgets.thirdPersonCamera.fadeOut(0.1f, 0.5f);
  widgets.coreUsageRenderer.fadeOut(0.2f, 0.5f);
  widgets.fitnessDataRenderer.fadeOut(0.3f, 0.5f);
  widgets.informationTextRenderer.fadeOut(0.3f, 0.5f);
}

void
State_Running::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  auto& context = Context::get();
  auto& logic = context.logic;
  auto& simulation = *logic.simulation;
  auto& graphic = context.graphic;

  { // simulation update

    auto& frameHandler = logic.carDataFrameHandler;

    frameHandler.update(elapsedTime);

    if (frameHandler.needNewFrame()) {

      if (simulation.isGenerationComplete() == false) {
        const float fakeElapsedTime = frameHandler.getLogicFrameDuration();
        const unsigned int totalSteps = (logic.isAccelerated ? 50 : 1);

        simulation.update(fakeElapsedTime, totalSteps);
      } else if (frameHandler.getTotalStoredFrames() <= 2) {
        simulation.breed();
      }
    }

  } // simulation update

  // done to avoid a spurious change of camera
  // -> which happen when changing states: Running -> EndGeneration
  if (StateManager::get()->getState() == StateManager::States::Running) {

    auto& sceneRenderer = graphic.renderer.getSceneRenderer();

    sceneRenderer.getModelsRenderer().update(elapsedTime);

    _updateCameraTracking(elapsedTime);
    _updateCommonLogic(elapsedTime);

    if (!simulation.isGenerationComplete()) {
      sceneRenderer.getAnimatedCircuitRenderer().update(elapsedTime);
    }
  }
}
