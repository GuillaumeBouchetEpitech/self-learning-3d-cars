
#pragma once

#include "application/defines.hpp"

#if defined D_WEB_WEBWORKER_BUILD

#include "State_AbstractSimulation.hpp"

#include "geronimo/system/Timer.hpp"

class State_WebWorkersLoading : public State_AbstractSimulation {
private:
  bool _awaiting = true;
  gero::Timer _timer;

public:
  void enter() override;

public:
  void handleEvent(const SDL_Event&) override;
  void update(float) override;
  void render(const SDL_Window&) override;

  void visibility(bool visible) override;
};

#endif
