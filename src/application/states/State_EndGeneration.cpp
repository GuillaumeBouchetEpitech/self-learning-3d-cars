
#include "State_EndGeneration.hpp"

#include "StateManager.hpp"

#include "application/context/graphics/Scene.hpp"
#include "application/context/Context.hpp"

void
State_EndGeneration::enter() {
  _timer.start(0.75f);
}

void
State_EndGeneration::update(float elapsedTime) {
  State_AbstractSimulation::update(elapsedTime);

  _updateCommonLogic(elapsedTime);

  auto& scene = Context::get().graphic.scene;
  scene.animatedCircuitRenderer.update(elapsedTime * 0.25f);

  _timer.update(elapsedTime);
  if (_timer.isDone()) {
    StateManager::get()->changeState(StateManager::States::StartGeneration);
  }
}
