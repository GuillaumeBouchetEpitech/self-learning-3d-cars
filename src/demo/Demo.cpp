
#include "Demo.hpp"

#include "demo/defines.hpp"
#include "demo/logic/Context.hpp"
#include "demo/logic/graphic/Scene.hpp"
#include "demo/states/StateManager.hpp"

#include "geronimo/system/TraceLogger.hpp"

#include <chrono>

namespace {

#if defined D_NATIVE_PTHREAD_BUILD
constexpr bool k_canResize = true;
#else
constexpr bool k_canResize = false;
#endif
} // namespace

Demo::Demo(const Definition& def)
  : gero::graphic::SDLWindowWrapper(
      "Self Learning 3d Cars", def.width, def.height, 30,
      SDLWindowWrapper::OpenGlEsVersion::v3, k_canResize) {
  Context::create(def.width, def.height, def.totalCores, def.genomesPerCore);
  StateManager::create();
  Scene::initialise();
}

Demo::~Demo() {
  StateManager::destroy();
  Context::destroy();
}

//

void
Demo::_onEvent(const SDL_Event& event) {
  StateManager::get()->handleEvent(event);
}

void
Demo::_onUpdate(uint32_t deltaTime) {

  auto& perfProfiler = Context::get().logic.metrics.performanceProfiler;
  perfProfiler.start("Update");

  const float elapsedTime = float(deltaTime) / 1000.0f;
  StateManager::get()->update(elapsedTime);

  perfProfiler.stop("Update");
}

void
Demo::_onRender(const SDL_Window& screen) {

  auto& perfProfiler = Context::get().logic.metrics.performanceProfiler;
  perfProfiler.start("Render");

  StateManager::get()->render(screen);

  perfProfiler.stop("Render");

  perfProfiler.stop("Frame");
  perfProfiler.start("Frame");
}

void
Demo::_onResize(uint32_t width, uint32_t height) {
  StateManager::get()->resize(width, height);
}

void
Demo::_onVisibilityChange(bool visible) {
  StateManager::get()->visibility(visible);
}
