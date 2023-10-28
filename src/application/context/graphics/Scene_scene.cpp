
#include "Scene.hpp"

#include "geronimo/graphics/GlContext.hpp"

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
Scene::_renderLeadingCarSensors() {
  auto& context = Context::get();
  const auto& leaderCar = context.logic.leaderCar;
  auto& renderer = context.graphic.renderer;
  auto& stackRenderer = renderer.getSceneRenderer().getSceneStackRenderers().getWireFramesStack();

  if (auto leaderCarData = leaderCar.leaderData()) {
    // leading car alive?
    if (!leaderCarData->isAlive)
      return;

    // do not show the sensor until far enough
    if (leaderCarData->groundIndex < 2)
      return;

    constexpr float colorIntensity = 0.8f;

    const glm::vec3 greenColor = glm::vec3(0.0f, 1.0f, 0.0f) * colorIntensity;
    const glm::vec3 yellowColor = glm::vec3(1.0f, 1.0f, 0.0f) * colorIntensity;
    const glm::vec3 orangeColor = glm::vec3(1.0f, 0.5f, 0.0f) * colorIntensity;
    const glm::vec3 redColor = glm::vec3(1.0f, 0.0f, 0.0f) * colorIntensity;
    const glm::vec3 whiteColor = glm::vec3(1.0f, 1.0f, 1.0f) * colorIntensity;

    // eye sensors
    for (const auto& sensor : leaderCarData->eyeSensors) {
      constexpr float redThreshold = 1.0f / 8.0f;
      constexpr float orangeThreshold = 1.0f / 4.0f;
      constexpr float yellowThreshold = 1.0f / 2.0f;

      glm::vec3 color = greenColor;
      if (sensor.value < redThreshold)
        color = redColor;
      else if (sensor.value < orangeThreshold)
        color = orangeColor;
      else if (sensor.value < yellowThreshold)
        color = yellowColor;

      stackRenderer.pushLine(sensor.near, sensor.far, color);
      stackRenderer.pushCross(sensor.far, color, 1.0f);
    }

    // ground sensor
    const auto& groundSensor = leaderCarData->groundSensor;
    stackRenderer.pushLine(groundSensor.near, groundSensor.far, whiteColor);
    stackRenderer.pushCross(groundSensor.far, whiteColor, 1.0f);
  }
}

void
Scene::renderScene(const gero::graphics::ICamera& inCamera) {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  // auto& scene = graphic.scene;

  auto& sceneRenderer = graphic.renderer.getSceneRenderer();

  sceneRenderer.setCamera(inCamera);


  {
    sceneRenderer.getBackGroundTorusRenderer().render();
    GlContext::clears(Buffers::depth);

    sceneRenderer.getChessBoardFloorRenderer().render();

    sceneRenderer.getAnimatedCircuitRenderer().renderWireFrame();

    Scene::_renderLeadingCarSensors();

    sceneRenderer.getSceneStackRenderers().flush();

    sceneRenderer.getParticleManager().render();

    sceneRenderer.getSceneStackRenderers().flush();

    sceneRenderer.getModelsRenderer().render();
    sceneRenderer.getCarTailsRenderer().render();
    sceneRenderer.getFlockingManager().render();

    sceneRenderer.getSceneStackRenderers().flush();

    sceneRenderer.getShapeStackRenderer().render();

    sceneRenderer.getAnimatedCircuitRenderer().renderGround();

    GlContext::disable(States::depthTest);

    sceneRenderer.getAnimatedCircuitRenderer().renderWalls();

    GlContext::enable(States::depthTest);
  }
}
