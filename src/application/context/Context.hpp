
#pragma once

#include "application/context/simulation/AbstractSimulation.hpp"
#include "application/states/StateManager.hpp"

#include "graphics/renderers/TrianglesStackRenderer.hpp"
#include "graphics/renderers/WireFramesStackRenderer.hpp"

#include "helpers/CarWheelsTrails.hpp"
#include "helpers/FitnessStats.hpp"
#include "helpers/LeaderCar.hpp"
#include "helpers/ProfileData.hpp"

#include "graphics/renderers/hud/CoreUsageRenderer.hpp"
#include "graphics/renderers/hud/FitnessDataRenderer.hpp"
#include "graphics/renderers/hud/InformationTextRenderer.hpp"
#include "graphics/renderers/hud/LeaderEyeRenderer.hpp"
#include "graphics/renderers/hud/PostProcess.hpp"
#include "graphics/renderers/hud/ScreenTitles.hpp"
#include "graphics/renderers/hud/TextRenderer.hpp"
#include "graphics/renderers/hud/ThirdPersonCamera.hpp"
#include "graphics/renderers/hud/TopologyRenderer.hpp"

#include "graphics/renderers/scene/AnimatedCircuitRenderer.hpp"
#include "graphics/renderers/scene/BackGroundTorusRenderer.hpp"
#include "graphics/renderers/scene/CarTailsRenderer.hpp"
#include "graphics/renderers/scene/FlockingManager.hpp"
#include "graphics/renderers/scene/FloorRenderer.hpp"
#include "graphics/renderers/scene/ModelsRenderer.hpp"
#include "graphics/renderers/scene/ParticleManager.hpp"

#include "geronimo/graphics/ResourceManager.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/metrics/PerformanceProfiler.hpp"

#include <array>
#include <list>
#include <memory> // <= unique_ptr / make_unique
#include <string>
#include <unordered_map>
#include <chrono>


class Context : public gero::NonCopyable {

  //
  //
  // singleton

private:
  static Context* _instance;

  Context() = default;
  ~Context();

private:
  void _initialise(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores);

public:
  static void create(uint32_t width, uint32_t height, uint32_t totalGenomes, uint32_t totalCores);
  static void destroy();
  static Context& get();

  // singleton
  //
  //

private:
  void _initialiseGraphicResource();
  void _initialiseSimulation(uint32_t totalGenomes, uint32_t totalCores);
  void _initialiseSimulationCallbacks();

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

    struct StackRenderers {
      WireFramesStackRenderer wireFrames;
      TrianglesStackRenderer triangles;
    };

    struct Hud {
      StackRenderers stackRenderers;
      TextRenderer textRenderer;
      TopologyRenderer topologyRenderer;
      ScreenTitles screenTitles;
      CoreUsageRenderer coreUsageRenderer;
      ThirdPersonCamera thirdPersonCamera;
      FitnessDataRenderer fitnessDataRenderer;
      InformationTextRenderer informationTextRenderer;
      LeaderEyeRenderer leaderEyeRenderer;
      PostProcess postProcess;
    } hud;

    struct Scene {
      StackRenderers stackRenderers;
      ParticleManager particleManager;
      FloorRenderer floorRenderer;
      BackGroundTorusRenderer backGroundTorusRenderer;
      AnimatedCircuitRenderer animatedCircuitRenderer;
      ModelsRenderer modelsRenderer;
      FlockingManager flockingManager;
      CarTailsRenderer carTailsRenderer;
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
