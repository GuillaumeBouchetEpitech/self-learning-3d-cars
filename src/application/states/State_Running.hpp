
#pragma once

#include "State_AbstractSimulation.hpp"

class State_Running : public State_AbstractSimulation {

public:
  void enter() override;
  void leave() override;

public:
  void update(float) override;
};
