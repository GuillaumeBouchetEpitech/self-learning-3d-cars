
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"

void
State_EndGeneration::enter() {
  _timer.start(0.75f);
}

void
State_EndGeneration::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  _updateCommonLogic(elapsedTime);

  Context::get().graphic.renderer.getSceneRenderer().getAnimatedCircuitRenderer().update(elapsedTime * 0.25f);

  _timer.update(elapsedTime);
  if (_timer.isDone()) {
    StateManager::get()->changeState(StateManager::States::StartGeneration);
  }
}
