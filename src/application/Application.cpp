
#include "Application.hpp"

#include "application/defines.hpp"
#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"
#include "application/states/StateManager.hpp"

#include "geronimo/system/TraceLogger.hpp"

#include <chrono>

namespace {

#if defined D_NATIVE_PTHREAD_BUILD
constexpr bool k_canResize = true;
#else
constexpr bool k_canResize = false;
#endif
} // namespace

using gero::graphics::SDLWindowWrapper;

Application::Application(const Definition& def)
  : SDLWindowWrapper("Self Learning 3d Cars", def.width, def.height, 30, OpenGlEsVersion::v3, k_canResize)
{
  Context::create(def.width, def.height, def.totalGenomes, def.totalCores);
  StateManager::create();
  Scene::initialise();
}

Application::~Application() {
  StateManager::destroy();
  Context::destroy();
}

//

void
Application::_onEvent(const SDL_Event& event) {
  StateManager::get()->handleEvent(event);
}

void
Application::_onUpdate(uint32_t deltaTime) {

  auto& perfProfiler = Context::get().logic.metrics.performanceProfiler;
  perfProfiler.start("Update");

  const float elapsedTime = float(deltaTime) / 1000.0f;
  StateManager::get()->update(elapsedTime);

  perfProfiler.stop("Update");
}

void
Application::_onRender(const SDL_Window& screen) {

  auto& perfProfiler = Context::get().logic.metrics.performanceProfiler;
  perfProfiler.start("Render");

  StateManager::get()->render(screen);

  perfProfiler.stop("Render");

  perfProfiler.stop("Frame");
  perfProfiler.start("Frame");
}

void
Application::_onResize(uint32_t width, uint32_t height) {
  StateManager::get()->resize(width, height);
}

void
Application::_onVisibilityChange(bool visible) {
  StateManager::get()->visibility(visible);
}
