
#include "CarTailsRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/asValue.hpp"

void
CarTailsRenderer::initialize() {

  const std::string basePath = "./src/application/context/graphics/renderers/scene/car-tails/shaders/";

  gero::graphics::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphics::GeometryBuilder geometryBuilder;

  shaderProgramBuilder.reset()
    .setVertexFilename(basePath + "wireFrames.glsl.vert")
    .setFragmentFilename(basePath + "wireFrames.glsl.frag")
    .addAttribute("a_vertex_position")
    .addUniform("u_composedMatrix")
    .addUniform("u_color");

  // gero::graphics::ShaderProgram::buildUnbound
  _shader = std::make_shared<gero::graphics::ShaderProgram>(shaderProgramBuilder.getDefinition());

  geometryBuilder.reset()
    .setShader(*_shader)
    .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::line_strip)
    .addVbo()
    .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f);

  _geometries.leaderCarTrail.initialize(*_shader, geometryBuilder.getDefinition());
  _geometries.leaderCarTrail.setPrimitiveCount(0);

  for (auto& wheelsTrail : _geometries.bestNewCarsTrails)
    for (auto& wheel : wheelsTrail.wheels) {
      wheel.initialize(*_shader, geometryBuilder.getDefinition());
      wheel.setPrimitiveCount(0);
    }
}

void
CarTailsRenderer::setMatricesData(const gero::graphics::Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void
CarTailsRenderer::updateLatestTrail() {
  auto& context = Context::get();
  auto& logic = context.logic;

  const auto& bestGenome = logic.simulation->getBestGenome();

  const auto& bestWheelsTrailData = logic.carWheelsTrails.getTrailById(bestGenome.getId());

  auto& currCarNewTrail = _geometries.bestNewCarsTrails.at(_currentTrailIndex);

  for (std::size_t ii = 0; ii < currCarNewTrail.wheels.size(); ++ii) {
    auto& currWheel = currCarNewTrail.wheels.at(ii);
    currWheel.updateOrAllocateBuffer(0, bestWheelsTrailData.wheels.at(ii));
    currWheel.setPrimitiveCount(bestWheelsTrailData.wheels.at(ii).size());
  }

  // increase the currently used trail index (loop if too high)
  _currentTrailIndex = (_currentTrailIndex + 1) % _geometries.bestNewCarsTrails.size();
}

void
CarTailsRenderer::render() {

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  _shader->preBind([this](gero::graphics::IBoundShaderProgram& bound) {
    bound.setUniform("u_composedMatrix", _matricesData.composed);
    bound.setUniform("u_color", 0.8f, 0.8f, 0.8f, 1.0f);

    auto& logic = Context::get().logic;

    { // best trails

      for (const auto& currentCarTrail : _geometries.bestNewCarsTrails)
        for (const auto& wheelTrail : currentCarTrail.wheels)
          wheelTrail.render();

    } // best trails

    { // leader trail

      if (logic.leaderCar.hasLeader() && !logic.carWheelsTrails.isEmpty()) {

        const auto& trailData = logic.carWheelsTrails.getTrailByIndex(logic.leaderCar.leaderIndex());
        const uint32_t totalStoredFrames = logic.carDataFrameHandler.getTotalStoredFrames();

        // rely on only the 30 last positions recorded
        constexpr int maxSize = 30;

        for (const auto& currWheel : trailData.wheels) {
          if (currWheel.size() < 10)
            continue;

          const int startIndex = std::max(0, int(totalStoredFrames) - 1);

          const int totalSize = currWheel.size();
          const int currSize = std::min(totalSize, maxSize) - startIndex;

          const float* dataPointer = &currWheel.at(totalSize - currSize - startIndex).x;
          const int dataSize = currSize * sizeof(CarWheelsTrailsHandler::WheelTrail::value_type);

          _geometries.leaderCarTrail.updateOrAllocateBuffer(0, dataSize, dataPointer);
          _geometries.leaderCarTrail.setPrimitiveCount(currSize);
          _geometries.leaderCarTrail.render();
        }
      }

    } // leader trail
  });
}
