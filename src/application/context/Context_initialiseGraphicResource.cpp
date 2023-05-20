
#include "Context.hpp"

#include "graphics/graphicsAliases.hpp"

#include "geronimo/system/asValue.hpp"

#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"

using namespace gero::graphics;

namespace {

void
_initializeSceneStructures(ResourceManager& rManager) {
  const std::string basePath = "./assets/graphics/shaders/scene/";

  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "wireFrames.glsl.vert")
      .setFragmentFilename(basePath + "wireFrames.glsl.frag")
      .addAttribute("a_vertex_position")
      .addUniform("u_composedMatrix")
      .addUniform("u_color");

    auto shader =
      rManager.createShader(gero::asValue(ShadersAliases::wireFrames), shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::wireFrames), geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::line_strip);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::wireFramesLineStrip), geometryBuilder.getDefinition(), true);
  }
}

} // namespace

void
Context::_initializeGraphicResource() {
  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  auto& rManager = graphic.resourceManager;

  _initializeSceneStructures(rManager);
}
