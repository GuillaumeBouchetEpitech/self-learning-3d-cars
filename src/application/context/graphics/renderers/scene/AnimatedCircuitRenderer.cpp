
#include "AnimatedCircuitRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"
#include "geronimo/system/easing/easingFunctions.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/helpers/GLMath.hpp"

using namespace gero::graphics;

void
AnimatedCircuitRenderer::initialize(
  const std::vector<glm::vec3>& skeletonVertices, const AnimatedVertices& groundVertices,
  const AnimatedVertices& wallsVertices, float maxUpperValue) {
  _maxUpperValue = maxUpperValue;

  auto& resourceManager = Context::get().graphic.resourceManager;

  const std::string basePath = "./assets/graphics/shaders/scene/animated-circuit/";

  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  shaderProgramBuilder.reset()
    .setVertexFilename(basePath + "animatedCircuitGround.glsl.vert")
    .setFragmentFilename(basePath + "animatedCircuitGround.glsl.frag")
    .addAttribute("a_vertex_position")
    .addAttribute("a_vertex_color")
    .addAttribute("a_vertex_normal")
    .addAttribute("a_vertex_animatedNormal")
    .addAttribute("a_vertex_index")
    .addUniform("u_composedMatrix")
    .addUniform("u_lightPos")
    // .addUniform("u_viewPos")
    .addUniform("u_alpha")
    .addUniform("u_lowerLimit")
    .addUniform("u_upperLimit");

  _shaderCircuitLit = std::make_shared<ShaderProgram>(shaderProgramBuilder.getDefinition());

  shaderProgramBuilder.reset()
    .setVertexFilename(basePath + "animatedCircuitWalls.glsl.vert")
    .setFragmentFilename(basePath + "animatedCircuitWalls.glsl.frag")
    .addAttribute("a_vertex_position")
    .addAttribute("a_vertex_color")
    .addAttribute("a_vertex_animatedNormal")
    .addAttribute("a_vertex_index")
    .addUniform("u_composedMatrix")
    .addUniform("u_alpha")
    .addUniform("u_lowerLimit")
    .addUniform("u_upperLimit");

  _shaderCircuit = std::make_shared<ShaderProgram>(shaderProgramBuilder.getDefinition());

  _shaderWireFrame = resourceManager.getShader(gero::asValue(ShadersAliases::wireFrames));

  { // compute circuit skeleton wireFrame geometry

    auto geoDef = resourceManager.getGeometryDefinition(gero::asValue(GeometriesAliases::wireFrames));
    _geometries.skeleton.initialize(*_shaderWireFrame, geoDef);
    _geometries.skeleton.allocateBuffer(0, skeletonVertices);
    _geometries.skeleton.setPrimitiveCount(skeletonVertices.size());

  } // compute circuit skeleton wireFrame geometry

  { // compute circuit ground geometries

    geometryBuilder.reset()
      .setShader(*_shaderCircuitLit)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_animatedNormal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_index", Geometry::AttrType::Float);

    _geometries.grounds.initialize(*_shaderCircuitLit, geometryBuilder.getDefinition());
    _geometries.grounds.allocateBuffer(0, groundVertices);
    _geometries.grounds.setPrimitiveCount(groundVertices.size());

  } // compute circuit ground geometries

  { // compute circuit walls geometries

    geometryBuilder.reset()
      .setShader(*_shaderCircuit)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_animatedNormal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_index", Geometry::AttrType::Float);

    _geometries.walls.initialize(*_shaderCircuit, geometryBuilder.getDefinition());
    _geometries.walls.allocateBuffer(0, wallsVertices);
    _geometries.walls.setPrimitiveCount(wallsVertices.size());

  } // compute circuit walls geometries

  _maxPrimitiveCount = groundVertices.size();

  int32_t lastIndex = 0;
  _circuitIndices.reserve(100);
  for (std::size_t ii = 0; ii + 3 < groundVertices.size(); ii += 3) {

    const float rawFloat = groundVertices.at(ii + 0).limitId;
    const int32_t currIndex = int32_t(std::ceil(rawFloat));
    if (currIndex != lastIndex + 1) {
      continue;
    }

    _circuitIndices.push_back(ii);

    lastIndex = uint32_t(currIndex);
  }

}

void
AnimatedCircuitRenderer::update(float elapsedTime) {
  auto& logic = Context::get().logic;
  const auto& simulation = *logic.simulation;

  if (logic.isAccelerated) {
    _targetValue = _maxUpperValue;
    _lowerValue = _maxUpperValue;
    _upperValue = _maxUpperValue;
  } else {
    _targetValue = 1.1f; // <= default value

    int bestGroundIndex = -1;
    for (unsigned int ii = 0; ii < simulation.getTotalCars(); ++ii) {
      const auto& carData = simulation.getCarResult(ii);

      if (!carData.isAlive || bestGroundIndex > carData.groundIndex)
        continue;

      bestGroundIndex = carData.groundIndex;
    }

    // do we have a car to focus the gero::graphics::Camera on?
    if (bestGroundIndex >= 0)
      _targetValue += bestGroundIndex;

    // lower value, closest from the cars

    const auto localEasing = gero::easing::GenericEasing<3>()
      .push(0.0f, 1.0f, gero::easing::easeOutCubic)
      .push(0.5f, 0.75f, gero::easing::easeOutCubic)
      .push(1.0f, 0.5f, gero::easing::easeOutCubic);

    if (_lowerValue > _targetValue) {

      const float tmpCoef = localEasing.get(_lowerValue - _targetValue);

      if (_lowerValue > _targetValue + 10.0f) {
        // fall really quickly
        _lowerValue -= 15.0f * elapsedTime * tmpCoef;
        _lowerValue = std::max(_lowerValue, _targetValue);
      } else {
        // fall quickly
        _lowerValue -= 4.5f * elapsedTime * tmpCoef;
        _lowerValue = std::max(_lowerValue, _targetValue);
      }
    } else {

      const float tmpCoef = localEasing.get(_targetValue - _lowerValue);

      // rise slowly
      _lowerValue += 3.0f * elapsedTime * tmpCoef;
      _lowerValue = std::min(_lowerValue, _targetValue);
    }

    // upper value, farthest from the cars

    const float newTargetValue = _targetValue + 0.4f;

    if (_upperValue > newTargetValue) {

      const float tmpCoef = localEasing.get(newTargetValue - _upperValue);

      if (_upperValue > newTargetValue + 10.0f) {
        // fall really quickly
        _upperValue -= 9.0f * elapsedTime * tmpCoef;
        _upperValue = std::max(_upperValue, newTargetValue);
      } else {
        // fall slowly
        _upperValue -= 1.5f * elapsedTime * tmpCoef;
        _upperValue = std::max(_upperValue, newTargetValue);
      }
    } else {

      const float tmpCoef = localEasing.get(_upperValue - newTargetValue);

      // rise really quickly
      _upperValue += 15.0f * elapsedTime * tmpCoef;
      _upperValue = std::min(_upperValue, newTargetValue);
    }
  }

  int32_t indexValue = _circuitIndices.at(std::min(int32_t(std::ceil(_upperValue)), int32_t(_circuitIndices.size()) - 1));
  if (indexValue > int32_t(_maxPrimitiveCount))
    indexValue = int32_t(_maxPrimitiveCount);

  _geometries.grounds.setPrimitiveCount(indexValue);
  _geometries.walls.setPrimitiveCount(indexValue * 2); // <= 2 walls
}

void
AnimatedCircuitRenderer::renderWireFrame(const gero::graphics::Camera& inCamera) {
  if (!_shaderWireFrame)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& matricesData = inCamera.getMatricesData();

  _shaderWireFrame->bind();
  _shaderWireFrame->setUniform("u_composedMatrix", matricesData.composed);
  _shaderWireFrame->setUniform("u_color", 0.6f, 0.6f, 0.6f, 1.0f);

  _geometries.skeleton.render();
}

void
AnimatedCircuitRenderer::renderWalls(const gero::graphics::Camera& inCamera) {
  if (!_shaderCircuit)
    D_THROW(std::runtime_error, "shader not setup");

  GlContext::disable(GlContext::States::depthTest);

  const auto& matricesData = inCamera.getMatricesData();

  _shaderCircuit->bind();
  _shaderCircuit->setUniform("u_composedMatrix", matricesData.composed);
  _shaderCircuit->setUniform("u_lowerLimit", _lowerValue);
  _shaderCircuit->setUniform("u_upperLimit", _upperValue);
  _shaderCircuit->setUniform("u_alpha", 0.2f);

  _geometries.walls.render();

  GlContext::enable(GlContext::States::depthTest);
}

void
AnimatedCircuitRenderer::renderGround(const gero::graphics::Camera& inCamera) {
  if (!_shaderCircuit)
    D_THROW(std::runtime_error, "shader not setup");

  const auto& matricesData = inCamera.getMatricesData();

  _shaderCircuitLit->bind();
  _shaderCircuitLit->setUniform("u_composedMatrix", matricesData.composed);
  _shaderCircuitLit->setUniform("u_lowerLimit", _lowerValue);
  _shaderCircuitLit->setUniform("u_upperLimit", _upperValue);
  _shaderCircuitLit->setUniform("u_alpha", 0.8f);
  _shaderCircuitLit->setUniform("u_lightPos", inCamera.getEye());
  // _shaderCircuitLit->setUniform("u_viewPos", inCamera.getEye());

  _geometries.grounds.render();
}
