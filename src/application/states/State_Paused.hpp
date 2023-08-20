
#pragma once

#include "State_AbstractSimulation.hpp"

class State_Paused : public State_AbstractSimulation {
public:
  void handleEvent(const SDL_Event&) override;
  void update(float) override;
  void visibility(bool visible) override;
};
