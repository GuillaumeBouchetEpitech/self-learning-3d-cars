
#pragma once

#include "State_AbstractSimulation.hpp"

#include "geronimo/system/Timer.hpp"

class State_EndGeneration : public State_AbstractSimulation {
private:
  gero::Timer _timer;

public:
  void enter() override;

public:
  void update(float) override;
};
