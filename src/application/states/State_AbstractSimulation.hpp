
#pragma once

#include "IState.hpp"

class State_AbstractSimulation : public IState {
public:
  void enter() override;
  void leave() override;

public:
  void handleEvent(const SDL_Event&) override;
  void update(float) override;
  void render(const SDL_Window&) override;
  void resize(int width, int height) override;
  void visibility(bool visible) override;

protected:
  void _updateCommonLogic(float elapsedTime);
  void _updateCameraTracking(float elapsedTime);
};
