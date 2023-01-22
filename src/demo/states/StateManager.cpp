
#include "StateManager.hpp"

#include "demo/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD
#include "State_WebWorkersLoading.hpp"
#endif

#include "State_EndGeneration.hpp"
#include "State_Paused.hpp"
#include "State_Running.hpp"
#include "State_StartGeneration.hpp"

#include "geronimo/system/TraceLogger.hpp"

//
//
// singleton

StateManager* StateManager::_instance = nullptr;

StateManager::StateManager() {
  // allocate states

  _states.at(gero::asValue(States::Running)) =
    std::make_unique<State_Running>();
  _states.at(gero::asValue(States::Paused)) = std::make_unique<State_Paused>();
  _states.at(gero::asValue(States::StartGeneration)) =
    std::make_unique<State_StartGeneration>();
  _states.at(gero::asValue(States::EndGeneration)) =
    std::make_unique<State_EndGeneration>();

#if defined D_WEB_WEBWORKER_BUILD
  _states.at(gero::asValue(States::WorkersLoading)) =
    std::make_unique<State_WebWorkersLoading>();
#endif

#if defined D_WEB_WEBWORKER_BUILD
  _currentState = States::WorkersLoading;
#else
  _currentState = States::StartGeneration;
#endif

  _previousState = _currentState;
  _states.at(gero::asValue(_currentState))->enter();
}

//

void
StateManager::create() {
  if (!_instance)
    _instance = new StateManager();
}

void
StateManager::destroy() {
  delete _instance, _instance = nullptr;
}

StateManager*
StateManager::get() {
  return _instance;
}

// singleton
//
//

void
StateManager::changeState(States nextState) {
  _states.at(gero::asValue(_currentState))->leave();

  _previousState = _currentState;
  _currentState = nextState;

  _states.at(gero::asValue(_currentState))->enter();
}

void
StateManager::returnToPreviousState() {
  if (_currentState == _previousState)
    return;

  _states.at(gero::asValue(_currentState))->leave();

  _currentState = _previousState;

  _states.at(gero::asValue(_currentState))->enter();
}

StateManager::States
StateManager::getState() const {
  return _currentState;
}

void
StateManager::handleEvent(const SDL_Event& event) {
  _states.at(gero::asValue(_currentState))->handleEvent(event);
}

void
StateManager::update(float elapsedTime) {
  _states.at(gero::asValue(_currentState))->update(elapsedTime);
}

void
StateManager::render(const SDL_Window& window) {
  _states.at(gero::asValue(_currentState))->render(window);
}

void
StateManager::resize(int width, int height) {
  _states.at(gero::asValue(_currentState))->resize(width, height);
}

void
StateManager::visibility(bool visible) {
  _states.at(gero::asValue(_currentState))->visibility(visible);
}
