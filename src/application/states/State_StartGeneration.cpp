
#include "State_StartGeneration.hpp"

#include "StateManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <iomanip>

void
State_StartGeneration::enter() {
  auto& context = Context::get();

  const float lastFitness = context.logic.fitnessStats.get(-1);
  if (lastFitness > 0.0f) {
    _timer.start(2.5f);
  } else {
    _timer.start(1.5f);
  }
  context.graphic.renderer.getHudRenderer().getWidgets().screenTitles.fadeIn(0.0f, 0.75f);
}

void
State_StartGeneration::leave() {
  auto& renderer = Context::get().graphic.renderer;
  renderer.getHudRenderer().getWidgets().screenTitles.fadeOut(0.0f, 0.5f);
  renderer.getSceneRenderer().getModelsRenderer().fadeIn(0.25f, 1.0f);
}

void
State_StartGeneration::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  _updateCommonLogic(elapsedTime);
  _updateCameraTracking(elapsedTime);

  Context::get().graphic.renderer.getSceneRenderer().getAnimatedCircuitRenderer().update(elapsedTime * 0.25f);

  _timer.update(elapsedTime);
  if (_timer.isDone()) {
    StateManager::get()->changeState(StateManager::States::Running);
  }
}
