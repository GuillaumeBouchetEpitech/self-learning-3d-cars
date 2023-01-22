
#pragma once

#include "State_AbstractSimulation.hpp"

#include "geronimo/system/Timer.hpp"

class State_EndGeneration : public State_AbstractSimulation {
private:
  gero::Timer _timer;

public:
  virtual void enter() override;

public:
  virtual void update(float) override;
};
