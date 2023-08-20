
#include "Application.hpp"

#include "application/context/helpers/inputManagers/KeyboardManager.hpp"
#include "application/context/helpers/inputManagers/MouseManager.hpp"
#include "application/context/helpers/inputManagers/TouchManager.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/Scene.hpp"
#include "application/defines.hpp"
#include "application/states/StateManager.hpp"

#include "geronimo/system/TraceLogger.hpp"

#include <chrono>

namespace {

#if defined D_NATIVE_PTHREAD_BUILD
constexpr bool k_canResize = true;
constexpr uint32_t k_frameRate = 120;
#else
constexpr bool k_canResize = false;
constexpr uint32_t k_frameRate = 0;
#endif

} // namespace

using gero::graphics::SDLWindowWrapper;

Application::Application(const Definition& def)
  : SDLWindowWrapper("Self Learning 3d Cars", def.width, def.height, k_frameRate, OpenGlEsVersion::v3, k_canResize) {

  KeyboardManager::create();
  MouseManager::create();
  TouchManager::create();
  Context::create(def.width, def.height, def.totalGenomes, def.totalCores);
  StateManager::create();
  Scene::initialize();
}

Application::~Application() {
  StateManager::destroy();
  Context::destroy();
  TouchManager::destroy();
  MouseManager::destroy();
  KeyboardManager::destroy();
}

//

void
Application::_onEvent(const SDL_Event& event) {
  StateManager::get()->handleEvent(event);
}

void
Application::_onUpdate(uint32_t deltaTime) {

  auto& perfProfiler = Context::get().logic.metrics.performanceProfiler;
  perfProfiler.stop("Complete Frame");
  perfProfiler.start("Complete Frame");
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
