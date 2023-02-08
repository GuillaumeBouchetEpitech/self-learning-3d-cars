
#include "Context.hpp"

#include "application/defines.hpp"
#if defined D_WEB_WEBWORKER_BUILD
#include "application/context/simulation/webworker/WebWorkersSimulation.hpp"
#else
#include "application/context/simulation/pthread/PthreadSimulation.hpp"
#endif

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
Context::initialise(
  unsigned int width, unsigned int height, unsigned int totalCores,
  unsigned int genomesPerCore) {
  {
    graphic.cameraData.viewportSize = {width, height};

    graphic.cameraData.scene.setPerspective(70.0f, 0.1f, 1500.0f);

    graphic.cameraData.hud.setOrthographic(
      0.0f, float(width), 0.0f, float(height), -10.0f, +10.0f);
    graphic.cameraData.hud.lookAt(
      glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    graphic.cameraData.hud.computeMatrices();
  }

  initialiseGraphicResource();

  //
  //
  //

#if defined D_WEB_WEBWORKER_BUILD

  logic.simulation = std::make_unique<WebWorkersSimulation>();

#else

  logic.simulation = std::make_unique<PthreadSimulation>();

#endif

  initialiseSimulationCallbacks();
  initialiseSimulation(totalCores, genomesPerCore);

  { // compute the top left HUD text

    std::string glVersion = gero::graphics::GlContext::getVersion();
    if (glVersion.empty())
      glVersion = "unknown";

    std::stringstream sstr;
    sstr << "Graphic: " << glVersion << std::endl;

#if defined D_WEB_BUILD

    sstr << "Type: C++ (WebAssembly Build)" << std::endl;

#if defined D_WEB_PTHREAD_BUILD

    sstr << "Mode: pthread";

#else

    sstr << "Mode: webworker (as a fallback)";

#endif

#else

    sstr << "Type: C++ (Native Build)" << std::endl;
    sstr << "Mode: pthread";

#endif

    logic.hudText.header = sstr.str();

  } // compute the top left HUD text

  {

    graphic.scene.stackRenderers.wireframes.initialise(
      ShadersAliases::stackRendererScene, GeometriesAliases::stackRendererWireframesScene);
    graphic.scene.stackRenderers.triangles.initialise(
      ShadersAliases::stackRendererScene, GeometriesAliases::stackRendererTrianglesScene);
    graphic.scene.particleManager.initialise();
    graphic.scene.modelsRenderer.initialise();
    graphic.scene.flockingManager.initialise();
    graphic.scene.carTailsRenderer.initialise();
    const auto& dimension = logic.circuitDimension;
    const glm::vec3 boundariesSize = dimension.max - dimension.min;
    graphic.scene.floorRenderer.initialise(dimension.center, boundariesSize);
    graphic.scene.backGroundTorusRenderer.initialise();

    graphic.hud.stackRenderers.wireframes.initialise(
      ShadersAliases::stackRendererHud, GeometriesAliases::stackRendererWireframesHud);
    graphic.hud.stackRenderers.triangles.initialise(
      ShadersAliases::stackRendererHud, GeometriesAliases::stackRendererTrianglesHud);
    graphic.hud.topologyRenderer.initialise();
    graphic.hud.textRenderer.initialise();
    graphic.hud.thirdPersonCamera.initialise();
    graphic.hud.leaderEyeRenderer.initialise();

    graphic.hud.postProcess.initialise({width, height});
    graphic.hud.postProcess.setGeometry(
      glm::vec2(0, 0), glm::vec2(width, height), -2.0f);
  }
}

//

void
Context::create(
  unsigned int width, unsigned int height, unsigned int totalCores,
  unsigned int genomesPerCore) {
  if (_instance)
    D_THROW(std::runtime_error, "Context singleton already initialised");

  _instance = new Context();
  _instance->initialise(width, height, totalCores, genomesPerCore);
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
    D_THROW(std::runtime_error, "Context singleton not initialised");

  return *_instance;
}

// singleton
//
//
