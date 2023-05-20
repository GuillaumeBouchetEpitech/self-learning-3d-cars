
#pragma once

#include "application/defines.hpp"

#include "IState.hpp"

#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/asValue.hpp"

#include <array>
#include <memory>

class StateManager : public gero::NonCopyable {
  //
  //
  // singleton

private:
  static StateManager* _instance;

  StateManager();
  ~StateManager() = default;

public:
  static void create();
  static void destroy();
  static StateManager* get();

  // singleton
  //
  //

public:
  enum class States : int {
#if defined D_WEB_WEBWORKER_BUILD
    WorkersLoading = 0,
    Running,
#else
    Running = 0,
#endif
    Paused,
    StartGeneration,
    EndGeneration,

    Total
  };

private:
  std::array<std::unique_ptr<IState>, gero::asValue(States::Total)> _states;

  States _currentState;
  States _previousState;

public:
  void changeState(States nextState);
  void returnToPreviousState();
  States getState() const;

public:
  void handleEvent(const SDL_Event&);
  void update(float);
  void render(const SDL_Window&);
  void resize(int, int);
  void visibility(bool);
};
