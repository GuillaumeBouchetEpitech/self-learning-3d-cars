
#include "Scene.hpp"

#include "geronimo/graphics/GlContext.hpp"

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
Scene::_renderLeadingCarSensors() {
  auto& context = Context::get();
  const auto& leaderCar = context.logic.leaderCar;
  auto& stackRenderer = context.graphic.scene.stackRenderers.getWireFramesStack();

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
Scene::renderScene(const gero::graphics::Camera& inCamera) {
  auto& context = Context::get();
  auto& graphic = context.graphic;
  auto& scene = graphic.scene;

  {
    const auto& matricesData = inCamera.getMatricesData();

    scene.stackRenderers.setMatricesData(matricesData);
    scene.carTailsRenderer.setMatricesData(matricesData);
  }

  {
    scene.backGroundTorusRenderer.render(inCamera);
    GlContext::clears(Buffers::depth);

    scene.chessBoardFloorRenderer.render(inCamera);

    scene.animatedCircuitRenderer.renderWireFrame(inCamera);

    Scene::_renderLeadingCarSensors();

    scene.stackRenderers.flush();

    scene.particleManager.render(inCamera);

    scene.stackRenderers.flush();

    scene.modelsRenderer.render(inCamera);
    scene.carTailsRenderer.render();
    scene.flockingManager->render();

    scene.stackRenderers.flush();

    scene.shapeStackRenderer.render(inCamera);

    scene.animatedCircuitRenderer.renderGround(inCamera);

    GlContext::disable(States::depthTest);

    scene.animatedCircuitRenderer.renderWalls(inCamera);

    GlContext::enable(States::depthTest);
  }
}
