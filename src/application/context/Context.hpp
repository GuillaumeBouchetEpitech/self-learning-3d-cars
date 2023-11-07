
#pragma once

#include "application/context/logic/simulation/AbstractSimulation.hpp"
#include "application/context/logic/simulation/logic/CarData.hpp"

#include "application/states/StateManager.hpp"

#include "logic/handlers/CarDataFrameHandler.hpp"
#include "logic/handlers/CarWheelsTrailsHandler.hpp"
#include "logic/handlers/FitnessStatsHandler.hpp"
#include "logic/handlers/LeaderCarHandler.hpp"
#include "logic/handlers/ProfileDataHandler.hpp"

#include "graphics/renderers/GraphicsRenderer.hpp"

#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"
#include "geronimo/system/metrics/PerformanceProfiler.hpp"

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
  void _initializeSimulation(uint32_t totalGenomes, uint32_t totalCores);
  void _initializeSimulationCallbacks();

public:
  struct Graphic {
    // TODO: move in a class
    struct cameraData {

      struct Rotations {
        float theta = -2.5f;
        float phi = 0.5f;
      } rotations;

      glm::vec3 center = {0.0f, 0.0f, 0.0f};
      float distance = 100.0f;

    } cameraData;

    GraphicsRenderer renderer;

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
      ProfileDataHandler profileData;

      uint32_t totalGenomes = 0;
      uint32_t totalCores = 0;
    } cores;

    bool isAccelerated = false;
    float timeSinceLastFrame = 0.0f;

    LeaderCarHandler leaderCar;

    CarWheelsTrailsHandler carWheelsTrails;

    struct CircuitDimension {
      glm::vec3 min;
      glm::vec3 max;
      glm::vec3 center;
    } circuitDimension;

    struct HudText {
      std::string headerGpu;
      std::string headerType;
    } hudText;

    FitnessStatsHandler fitnessStats;
  } logic;
};
