
#include "ModelsRenderer.hpp"

#include "application/context/Context.hpp"
#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"

#include "geronimo/graphics/loaders/loadObjModel.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/rng/RandomNumberGenerator.hpp"

using namespace gero::graphics;

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

  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  const std::string basePath = "./assets/graphics/shaders/scene/";

  auto& context = Context::get();

  const uint32_t totalCars = context.logic.carDataFrameHandler.getAllCarsData().size();

  _modelsCarChassisMatrices.reserve(totalCars);    // pre-allocate
  _modelsCarWheelsMatrices.reserve(totalCars * 4); // pre-allocate

  const std::string modelBasePath = "./assets/graphics/model/";

  { // chassis gero::graphics::Geometry (instanced)

    gero::graphics::loader::ModelVertices modelVertices;
    gero::graphics::loader::loadObjModel(
      modelBasePath + "CarNoWheels.obj", modelBasePath, modelVertices);

    _updateVerticesNormals(modelVertices);

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "modelsCarChassis.glsl.vert")
      .setFragmentFilename(basePath + "modelsCarChassis.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_vertex_normal")
      .addAttribute("a_offset_position")
      .addAttribute("a_offset_orientation")
      .addAttribute("a_offset_scale")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineValue")
      .addUniform("u_composedMatrix")
      .addUniform("u_lightPos")
      .addUniform("u_viewPos");

    _chassis.shader = std::make_shared<ShaderProgram>(shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*_chassis.shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .setVboAsDynamic()
      .addVboAttribute("a_offset_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_orientation", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_scale", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineValue", Geometry::AttrType::Float);

    _chassis.geometry.initialize(*_chassis.shader, geometryBuilder.getDefinition());
    _chassis.geometry.allocateBuffer(0, modelVertices);
    _chassis.geometry.setPrimitiveCount(modelVertices.size());
    _chassis.geometry.preAllocateBufferFromCapacity(1, _modelsCarChassisMatrices);
  }

  { // wheel gero::graphics::Geometry (instanced)

    gero::graphics::loader::ModelVertices modelVertices;
    gero::graphics::loader::loadObjModel(
      modelBasePath + "CarWheel.obj", modelBasePath, modelVertices);

    _updateVerticesNormals(modelVertices); // TODO: useful?

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "modelsCarWheels.glsl.vert")
      .setFragmentFilename(basePath + "modelsCarWheels.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_offset_position")
      .addAttribute("a_offset_orientation")
      .addAttribute("a_offset_scale")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineValue")
      .addUniform("u_composedMatrix");

    _wheels.shader = std::make_shared<ShaderProgram>(shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*_wheels.shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .setVboAsDynamic()
      .addVboAttribute("a_offset_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_orientation", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_scale", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineValue", Geometry::AttrType::Float);

    _wheels.geometry.initialize(*_wheels.shader, geometryBuilder.getDefinition());
    _wheels.geometry.allocateBuffer(0, modelVertices);
    _wheels.geometry.setPrimitiveCount(modelVertices.size());
    _wheels.geometry.preAllocateBufferFromCapacity(1, _modelsCarWheelsMatrices);
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

  const uint32_t totalCars = logic.carDataFrameHandler.getAllCarsData().size();
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

  for (uint32_t ii = 0; ii < totalCars; ++ii) {
    const auto& carData = logic.carDataFrameHandler.getAllCarsData().at(ii);

    if (!carData.isAlive)
      continue;

    //
    // 3d clipping

    const auto& chassis = carData.liveTransforms.chassis;
    const glm::vec3 carOrigin =
      chassis.position + glm::mat3_cast(chassis.orientation) * modelHeight;

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
      chassis.position, chassis.orientation, k_scale, color);

    for (const auto& wheelTransform : carData.liveTransforms.wheels)
      _modelsCarWheelsMatrices.emplace_back(
        wheelTransform.position, wheelTransform.orientation, k_scale,
        k_whiteColor);
  }

  if (!_modelsCarChassisMatrices.empty()) {
    _chassis.shader->bind();
    _chassis.shader->setUniform("u_composedMatrix", matricesData.composed);
    _chassis.shader->setUniform("u_lightPos", inCamera.getEye());
    _chassis.shader->setUniform("u_viewPos", inCamera.getEye());

    _chassis.geometry.updateOrAllocateBuffer(1, _modelsCarChassisMatrices);
    _chassis.geometry.setInstancedCount(_modelsCarChassisMatrices.size());
    _chassis.geometry.render();
  }

  if (!_modelsCarWheelsMatrices.empty()) {
    _wheels.shader->bind();
    _wheels.shader->setUniform("u_composedMatrix", matricesData.composed);

    _wheels.geometry.updateOrAllocateBuffer(1, _modelsCarWheelsMatrices);
    _wheels.geometry.setInstancedCount(_modelsCarWheelsMatrices.size());
    _wheels.geometry.render();
  }
}
