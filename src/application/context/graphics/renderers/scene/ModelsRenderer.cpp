
#include "ModelsRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/loaders/loadObjModel.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

namespace {

void
_updateVerticesNormals(gero::graphics::loader::ModelVertices& vertices) {
  for (std::size_t index = 0; index < vertices.size(); index += 3) {
    gero::graphics::loader::ModelVertex& vertexA = vertices.at(index + 0);
    gero::graphics::loader::ModelVertex& vertexB = vertices.at(index + 1);
    gero::graphics::loader::ModelVertex& vertexC = vertices.at(index + 2);

    const glm::vec3 normal = glm::cross(
      vertexA.position - vertexB.position, vertexA.position - vertexC.position);

    vertexA.normal = normal;
    vertexB.normal = normal;
    vertexC.normal = normal;
  }
}

} // namespace

void
ModelsRenderer::initialize() {

  auto& resourceManager = Context::get().graphic.resourceManager;

  const std::string basePath = "./assets/graphics/model/";

  { // chassis gero::graphics::Geometry (instanced)

    gero::graphics::loader::ModelVertices modelVertices;
    gero::graphics::loader::loadObjModel(
      basePath + "CarNoWheels.obj", basePath, modelVertices);

    _updateVerticesNormals(modelVertices);

    _chassis.shader =
      resourceManager.getShader(gero::asValue(ShadersAliases::modelsCarChassis));

    auto geoDef = resourceManager.getGeometryDefinition(
      gero::asValue(GeometriesAliases::modelsCarChassis));
    _chassis.geometry.initialize(*_chassis.shader, geoDef);
    _chassis.geometry.updateBuffer(0, modelVertices);
    _chassis.geometry.setPrimitiveCount(modelVertices.size());
  }

  { // wheel gero::graphics::Geometry (instanced)

    gero::graphics::loader::ModelVertices modelVertices;
    gero::graphics::loader::loadObjModel(
      basePath + "CarWheel.obj", basePath, modelVertices);

    _updateVerticesNormals(modelVertices); // TODO: useful?

    _wheels.shader =
      resourceManager.getShader(gero::asValue(ShadersAliases::modelsCarWheels));

    auto geoDef = resourceManager.getGeometryDefinition(
      gero::asValue(GeometriesAliases::modelsCarWheels));
    _wheels.geometry.initialize(*_wheels.shader, geoDef);
    _wheels.geometry.updateBuffer(0, modelVertices);
    _wheels.geometry.setPrimitiveCount(modelVertices.size());
  }
}

void
ModelsRenderer::fadeIn(float delay, float duration) {
  _timer.start(delay, duration);
}

void
ModelsRenderer::update(float elapsedTime) {
  if (_timer.isDone())
    return;

  _timer.update(elapsedTime);

  _colorAlpha = gero::easing::GenericEasing<3>()
                  .push(0.0f, 0.0f)
                  .push(0.5f, 0.0f, gero::easing::easeOutCubic)
                  .push(1.0f, 1.0f)
                  .get(_timer.getCoefElapsed());

  _outlineColoralpha = gero::easing::GenericEasing<2>()
                         .push(0.0f, 0.0f, gero::easing::easeOutCubic)
                         .push(0.5f, 1.0f)
                         .get(_timer.getCoefElapsed());
}

void
ModelsRenderer::render(const gero::graphics::Camera& inCamera) {
  if (!_chassis.shader || !_wheels.shader)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& logic = Context::get().logic;
  const auto& simulation = *logic.simulation;

  const unsigned int totalCars = simulation.getTotalCars();
  if (totalCars == 0)
    return;

  const auto& frustumCulling = inCamera.getFrustumCulling();

  const auto& matricesData = inCamera.getMatricesData();

  _modelsCarChassisMatrices.clear();
  _modelsCarWheelsMatrices.clear();
  _modelsCarChassisMatrices.reserve(totalCars);    // pre-allocate
  _modelsCarWheelsMatrices.reserve(totalCars * 4); // pre-allocate

  const glm::vec3 modelHeight(0.0f, 0.0f, 0.2f);

  const glm::vec4 k_outlineColor(1, 1, 1, _outlineColoralpha);
  const glm::vec4 k_whiteColor(1, 1, 1, _colorAlpha);
  const glm::vec4 k_greenColor(0, 1, 0, _colorAlpha);
  const glm::vec4 k_redColor(1, 0, 0, _colorAlpha);
  const glm::vec4& leaderColor = k_whiteColor;
  const glm::vec4& lifeColor = k_greenColor;
  const glm::vec4& deathColor = k_redColor;

  for (unsigned int ii = 0; ii < totalCars; ++ii) {
    const auto& carData = simulation.getCarResult(ii);

    if (!carData.isAlive)
      continue;

    //
    // 3d clipping

    const auto& chassis = carData.liveTransforms.chassis;
    const glm::vec3 carOrigin = chassis.position + glm::mat3_cast(chassis.orientation) * modelHeight;

    if (!frustumCulling.sphereInFrustum(carOrigin, 5.0f))
      continue;

    //
    // color

    const bool isLeader = (logic.leaderCar.leaderIndex() == int(ii));
    const glm::vec4 targetColor = isLeader ? leaderColor : lifeColor;
    const glm::vec4 color = glm::mix(deathColor, targetColor, carData.life);

    //
    //

    const glm::vec3 k_scale = glm::vec3(1.0f);

    //
    // transforms

    _modelsCarChassisMatrices.emplace_back(
      chassis.position,
      chassis.orientation,
      k_scale,
      color);

    for (const auto& wheelTransform : carData.liveTransforms.wheels)
      _modelsCarWheelsMatrices.emplace_back(
        wheelTransform.position,
        wheelTransform.orientation,
        k_scale,
        k_whiteColor);
  }

  if (!_modelsCarChassisMatrices.empty()) {
    _chassis.shader->bind();
    _chassis.shader->setUniform("u_composedMatrix", matricesData.composed);
    _chassis.shader->setUniform("u_lightPos", inCamera.getEye());

    _chassis.geometry.updateBuffer(1, _modelsCarChassisMatrices);
    _chassis.geometry.setInstancedCount(_modelsCarChassisMatrices.size());
    _chassis.geometry.render();
  }

  if (!_modelsCarWheelsMatrices.empty()) {
    _wheels.shader->bind();
    _wheels.shader->setUniform("u_composedMatrix", matricesData.composed);

    _wheels.geometry.updateBuffer(1, _modelsCarWheelsMatrices);
    _wheels.geometry.setInstancedCount(_modelsCarWheelsMatrices.size());
    _wheels.geometry.render();
  }
}
