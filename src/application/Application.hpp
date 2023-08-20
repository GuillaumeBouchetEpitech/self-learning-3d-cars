
#pragma once

#include "geronimo/graphics/sdl/SDLWindowWrapper.hpp"

#include <cstdint>

class Application : public gero::graphics::SDLWindowWrapper {
public:
  struct Definition {
    uint32_t totalCores = 0;
    uint32_t totalGenomes = 0;
    uint32_t width = 0;
    uint32_t height = 0;
  };

public:
  Application(const Definition& def);
  ~Application();

private:
  void _onEvent(const SDL_Event& event) override;
  void _onUpdate(uint32_t deltaTime) override;
  void _onRender(const SDL_Window& screen) override;
  void _onResize(uint32_t width, uint32_t height) override;
  void _onVisibilityChange(bool visible) override;
};
