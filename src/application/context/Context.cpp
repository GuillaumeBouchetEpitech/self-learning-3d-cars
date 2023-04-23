
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
Context::_initialize(
  uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores) {
  {
    graphic.cameraData.viewportSize = {width, height};

    graphic.cameraData.scene.setPerspective(70.0f, 0.1f, 1500.0f);

    graphic.cameraData.hud.setOrthographic(
      0.0f, float(width), 0.0f, float(height), -10.0f, +10.0f);
    graphic.cameraData.hud.lookAt(
      glm::vec3(0, 0, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    graphic.cameraData.hud.computeMatrices();
  }

  _initializeGraphicResource();

  //
  //
  //

#if defined D_WEB_WEBWORKER_BUILD

  logic.simulation = std::make_unique<WebWorkersSimulation>();

#else

  logic.simulation = std::make_unique<PthreadSimulation>();

#endif

  logic.carDataFrameHandler.initalize(totalGenomes);

  _initializeSimulationCallbacks();
  _initializeSimulation(totalGenomes, totalCores);

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

    graphic.scene.stackRenderers.initialize(
      ShadersAliases::stackRendererScene,
      GeometriesAliases::stackRendererTrianglesScene,
      GeometriesAliases::stackRendererWireFramesScene);

    graphic.scene.modelsRenderer.initialize();
    graphic.scene.carTailsRenderer.initialize();
    const auto& dimension = logic.circuitDimension;
    const glm::vec3 boundariesSize = dimension.max - dimension.min;
    graphic.scene.chessBoardFloorRenderer.initialize(dimension.center, boundariesSize);
    graphic.scene.backGroundTorusRenderer.initialize();
    graphic.scene.geometriesStackRenderer.initialize();

    graphic.hud.stackRenderers.initialize(
      ShadersAliases::stackRendererScene,
      GeometriesAliases::stackRendererTrianglesHud,
      GeometriesAliases::stackRendererWireFramesHud);

    graphic.hud.textRenderer.initialize();

    graphic.hud.postProcess.initialize({width, height});
    graphic.hud.postProcess.setGeometry(
      glm::vec2(0, 0), glm::vec2(width, height), -2.0f);

    graphic.hud.widgets.topologyRenderer.initialize();
    graphic.hud.widgets.thirdPersonCamera.initialize();
    graphic.hud.widgets.leaderEyeRenderer.initialize();
  }

  {

    {
      //
      // sphere geometry
      //

      gero::graphics::MakeGeometries::Vertices vertices;

      gero::graphics::MakeGeometries::makeSphere(vertices, 0, 0.5f);
      // gero::graphics::MakeGeometries::convertToPerFacesNormals(vertices);
      graphic.scene.geometriesStackRenderer.createAlias(1111, vertices);

      gero::graphics::MakeGeometries::makeSphere(vertices, 1, 0.5f);
      // gero::graphics::MakeGeometries::convertToPerFacesNormals(vertices);
      graphic.scene.geometriesStackRenderer.createAlias(2222, vertices);

    }

  }
}

//

void
Context::create(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores)
{
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
