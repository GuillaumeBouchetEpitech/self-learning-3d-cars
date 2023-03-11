
#include "AnimatedCircuitRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/asValue.hpp"

using namespace gero::graphics;

void
AnimatedCircuitRenderer::initialise(
  const std::vector<glm::vec3>& skeletonVertices,
  const AnimatedVertices& groundVertices, const AnimatedVertices& wallsVertices,
  float maxUpperValue) {
  _maxUpperValue = maxUpperValue;

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shaderWireFrame =
    resourceManager.getShader(gero::asValue(ShadersAliases::wireFrames));
  _shaderCircuitLit =
    resourceManager.getShader(gero::asValue(ShadersAliases::animatedCircuitGround));
  _shaderCircuit =
    resourceManager.getShader(gero::asValue(ShadersAliases::animatedCircuitWalls));

  { // compute circuit skeleton wireFrame geometry

    auto geoDef = resourceManager.getGeometryDefinition(
      gero::asValue(GeometriesAliases::wireFrames));
    _geometries.skeleton.initialise(*_shaderWireFrame, geoDef);
    _geometries.skeleton.updateBuffer(0, skeletonVertices);
    _geometries.skeleton.setPrimitiveCount(skeletonVertices.size());

  } // compute circuit skeleton wireFrame geometry

  { // compute circuit ground geometries

    auto geoDef = resourceManager.getGeometryDefinition(
      gero::asValue(GeometriesAliases::animatedCircuitGround));
    _geometries.grounds.initialise(*_shaderCircuitLit, geoDef);
    _geometries.grounds.updateBuffer(0, groundVertices);
    _geometries.grounds.setPrimitiveCount(groundVertices.size());

  } // compute circuit ground geometries

  { // compute circuit walls geometries

    auto geoDef = resourceManager.getGeometryDefinition(
      gero::asValue(GeometriesAliases::animatedCircuitWalls));
    _geometries.walls.initialise(*_shaderCircuit, geoDef);
    _geometries.walls.updateBuffer(0, wallsVertices);
    _geometries.walls.setPrimitiveCount(wallsVertices.size());

  } // compute circuit walls geometries

  _maxPrimitiveCount = groundVertices.size();
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
    _targetValue = 3.0f; // <= default value

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

    if (_lowerValue > _targetValue + 10.0f) {
      // fall really quickly
      _lowerValue -= 60.0f * elapsedTime;
      _lowerValue = std::max(_lowerValue, _targetValue);
    } else if (_lowerValue > _targetValue) {
      // fall quickly
      _lowerValue -= 18.0f * elapsedTime;
      _lowerValue = std::max(_lowerValue, _targetValue);
    } else {
      // rise slowly
      _lowerValue += 12.0f * elapsedTime;
      _lowerValue = std::min(_lowerValue, _targetValue);
    }

    // upper value, farthest from the cars

    if (_upperValue > _targetValue + 10.0f) {
      // fall really quickly
      _upperValue -= 36.0f * elapsedTime;
      _upperValue = std::max(_upperValue, _targetValue);
    } else if (_upperValue > _targetValue) {
      // fall slowly
      _upperValue -= 6.0f * elapsedTime;
      _upperValue = std::max(_upperValue, _targetValue);
    } else {
      // rise really quickly
      _upperValue += 60.0f * elapsedTime;
      _upperValue = std::min(_upperValue, _targetValue);
    }
  }

  const unsigned int verticesLength = 36; // <= 3 * 12 triangles
  int indexValue = std::ceil(_upperValue) * verticesLength;
  if (indexValue > int(_maxPrimitiveCount))
    indexValue = int(_maxPrimitiveCount);

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

  _geometries.grounds.render();
}
