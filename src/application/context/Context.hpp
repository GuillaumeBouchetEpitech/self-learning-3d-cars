
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"
#include "application/context/simulation/logic/CarData.hpp"

#include "application/states/StateManager.hpp"

#include "helpers/CarDataFrameHandler.hpp"
#include "helpers/CarWheelsTrails.hpp"
#include "helpers/FitnessStats.hpp"
#include "helpers/LeaderCar.hpp"
#include "helpers/ProfileData.hpp"

#include "graphics/renderers/hud/PostProcess.hpp"
#include "graphics/renderers/hud/widgets/CoreUsageRenderer.hpp"
#include "graphics/renderers/hud/widgets/FitnessDataRenderer.hpp"
#include "graphics/renderers/hud/widgets/InformationTextRenderer.hpp"
#include "graphics/renderers/hud/widgets/LeaderEyeRenderer.hpp"
#include "graphics/renderers/hud/widgets/ScreenTitles.hpp"
#include "graphics/renderers/hud/widgets/ThirdPersonCamera.hpp"
#include "graphics/renderers/hud/widgets/TopologyRenderer.hpp"

#include "graphics/renderers/scene/AnimatedCircuitRenderer.hpp"
#include "graphics/renderers/scene/BackGroundTorusRenderer.hpp"
#include "graphics/renderers/scene/CarTailsRenderer.hpp"
#include "graphics/renderers/scene/ChessBoardFloorRenderer.hpp"
#include "graphics/renderers/scene/ModelsRenderer.hpp"
#include "graphics/renderers/scene/SceneStackRenderers.hpp"
#include "graphics/renderers/scene/flocking/AbstractFlockingManager.hpp"
#include "graphics/renderers/scene/particles/ParticleManager.hpp"
#include "graphics/renderers/scene/shape-stack-renderer/ShapeStackRenderer.hpp"

#include "geronimo/graphics/ResourceManager.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/metrics/PerformanceProfiler.hpp"

#include "geronimo/graphics/advanced-concept/stackRenderers/StackRenderers.hpp"
#include "geronimo/graphics/advanced-concept/textRenderer/TextRenderer.hpp"

#include <array>
#include <chrono>
#include <list>
#include <memory> // <= unique_ptr / make_unique
#include <string>
#include <unordered_map>

class Context : public gero::NonCopyable {

  //
  //
  // singleton

private:
  static Context* _instance;

  Context() = default;
  ~Context();

private:
  void _initialize(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores);

public:
  static void create(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores);
  static void destroy();
  static Context& get();

  // singleton
  //
  //

private:
  void _initializeGraphicResource();
  void _initializeSimulation(uint32_t totalGenomes, uint32_t totalCores);
  void _initializeSimulationCallbacks();

public:
  struct Graphic {
    // TODO: move in a class
    struct cameraData {
      glm::vec2 viewportSize = {800.0f, 600.0f};

      struct Rotations {
        float theta = -2.5f;
        float phi = 0.5f;
      } rotations;

      glm::vec3 center = {0.0f, 0.0f, 0.0f};
      float distance = 100.0f;

      gero::graphics::Camera scene;
      gero::graphics::Camera hud;
    } cameraData;

    struct Hud {
      gero::graphics::StackRenderers stackRenderers;
      gero::graphics::TextRenderer textRenderer;

      PostProcess postProcess;

      struct Widgets {
        TopologyRenderer topologyRenderer;
        ScreenTitles screenTitles;
        CoreUsageRenderer coreUsageRenderer;
        ThirdPersonCamera thirdPersonCamera;
        FitnessDataRenderer fitnessDataRenderer;
        InformationTextRenderer informationTextRenderer;
        LeaderEyeRenderer leaderEyeRenderer;
      } widgets;
    } hud;

    struct Scene {
      SceneStackRenderers stackRenderers;
      ParticleManager particleManager;
      ChessBoardFloorRenderer chessBoardFloorRenderer;
      BackGroundTorusRenderer backGroundTorusRenderer;
      AnimatedCircuitRenderer animatedCircuitRenderer;
      ModelsRenderer modelsRenderer;
      std::unique_ptr<AbstractFlockingManager> flockingManager;
      CarTailsRenderer carTailsRenderer;
      ShapeStackRenderer shapeStackRenderer;
    } scene;

    gero::graphics::ResourceManager resourceManager;
  } graphic;

  //

  struct Logic {
    struct Metrics {

      gero::metrics::PerformanceProfiler performanceProfiler;

    } metrics;

    NeuralNetworkTopology annTopology;

    std::unique_ptr<AbstractSimulation> simulation = nullptr;

    CarDataFrameHandler carDataFrameHandler;

    struct Cores {
      ProfileData profileData;

      uint32_t totalGenomes = 0;
      uint32_t totalCores = 0;
    } cores;

    bool isAccelerated = false;
    float timeSinceLastFrame = 0.0f;

    LeaderCar leaderCar;

    CarWheelsTrails carWheelsTrails;

    struct CircuitDimension {
      glm::vec3 min;
      glm::vec3 max;
      glm::vec3 center;
    } circuitDimension;

    struct HudText {
      std::string header;
    } hudText;

    FitnessStats fitnessStats;
  } logic;

  struct Input {
    std::unordered_map<int, bool> keys;

    struct Mouse {
      glm::vec2 position = {0, 0};
      glm::vec2 delta = {0, 0};
      bool tracking = false;
    } mouse;
  } inputs;
};
