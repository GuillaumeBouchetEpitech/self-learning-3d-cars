
#include "Context.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

#include <iomanip>
#include <sstream>

//
//
// singleton

Context* Context::_instance = nullptr;

Context::~Context() {}

void
Context::_initialize(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores) {

  //
  //
  //

  logic.metrics.performanceProfiler.setSize(150);

  logic.simulation = AbstractSimulation::create();

  constexpr float k_logicFrameDuration = 1.0f / 55.0f;
  logic.carDataFrameHandler.initialize(totalGenomes, k_logicFrameDuration);

  _initializeSimulationCallbacks();
  _initializeSimulation(totalGenomes, totalCores);

  { // compute the top left HUD text

    std::string glVersion = gero::graphics::GlContext::getVersion();
    if (glVersion.empty())
      glVersion = "unknown";

    std::stringstream sstr;
    sstr << glVersion << std::endl;

    logic.hudText.headerGpu = sstr.str();
  }

  {
    std::stringstream sstr;

#if defined D_WEB_BUILD

    sstr << "C++ (WebAssembly Build)" << std::endl;

#if defined D_WEB_PTHREAD_BUILD

    sstr << "PTHREAD";

#else

    sstr << "webworker (PTHREAD fallback)";

#endif

#else

    sstr << "C++ (Native Build)" << std::endl;
    sstr << "PTHREAD";

#endif

    logic.hudText.headerType = sstr.str();

  } // compute the top left HUD text

  {

    const auto& dimension = logic.circuitDimension;
    graphic.renderer.initialize(width, height, dimension.center, dimension.max - dimension.min);
  }
}

//

void
Context::create(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores) {
  if (_instance)
    D_THROW(std::runtime_error, "Context singleton already initialized");

  _instance = new Context();
  _instance->_initialize(width, height, totalGenomes, totalCores);
}

void
Context::destroy() {
  if (!_instance)
    D_THROW(std::runtime_error, "Context singleton already destroyed");

  delete _instance, _instance = nullptr;
}

Context&
Context::get() {
  if (!_instance)
    D_THROW(std::runtime_error, "Context singleton not initialized");

  return *_instance;
}

// singleton
//
//
