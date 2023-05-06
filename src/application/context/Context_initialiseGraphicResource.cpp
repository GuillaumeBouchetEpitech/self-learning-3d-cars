
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
      .setVertexFilename(basePath + "stackRenderer.glsl.vert")
      .setFragmentFilename(basePath + "stackRenderer.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::stackRendererScene),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .setVboAsDynamic()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererWireFramesScene),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererTrianglesScene),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "backGroundTorus.glsl.vert")
      .setFragmentFilename(basePath + "backGroundTorus.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture")
      .addUniform("u_animationCoef");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::backGroundTorus),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_texCoord", Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::backGroundTorus),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "simpleTexture.glsl.vert")
      .setFragmentFilename(basePath + "simpleTexture.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::simpleTexture),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_texCoord", Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::simpleTexture),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "chessboardFloor.glsl.vert")
      .setFragmentFilename(basePath + "chessboardFloor.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_normal")
      .addAttribute("a_vertex_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture")
      .addUniform("u_lightPos")
      .addUniform("u_viewPos");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::chessboardFloor),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_texCoord", Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::chessboardFloor),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "wireFrames.glsl.vert")
      .setFragmentFilename(basePath + "wireFrames.glsl.frag")
      .addAttribute("a_vertex_position")
      .addUniform("u_composedMatrix")
      .addUniform("u_color");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::wireFrames),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::wireFrames),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::line_strip);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::wireFramesLineStrip),
      geometryBuilder.getDefinition(), true);
  }

  {
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

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::animatedCircuitWalls),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_animatedNormal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_index", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::animatedCircuitWalls),
      geometryBuilder.getDefinition(), true);
  }

  {
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
      .addUniform("u_viewPos")
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::animatedCircuitGround),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_animatedNormal", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_index", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::animatedCircuitGround),
      geometryBuilder.getDefinition(), true);
  }

  {

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

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::modelsCarWheels),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
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

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::modelsCarWheels),
      geometryBuilder.getDefinition(), true);
  }

  {

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

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::modelsCarChassis),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
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

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::modelsCarChassis),
      geometryBuilder.getDefinition(), true);
  }

  {

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "geometriesStackRenderer.glsl.vert")
      .setFragmentFilename(basePath + "geometriesStackRenderer.glsl.frag")

      .addAttribute("a_vertex_position")
      // .addAttribute("a_vertex_normal")

      .addAttribute("a_offset_position")
      .addAttribute("a_offset_orientation")
      .addAttribute("a_offset_scale")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineValue")

      .addUniform("u_composedMatrix")
      // .addUniform("u_ambiantCoef")
      // .addUniform("u_lightPos")
      ;

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::geometriesStackRenderer),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      // .addVboAttribute("a_vertex_normal", Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .setVboAsDynamic()
      .addVboAttribute("a_offset_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_orientation", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_scale", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineValue", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::geometriesStackRenderer),
      geometryBuilder.getDefinition(), true);
  }
}

void
_initializeHudStructures(ResourceManager& rManager) {
  const std::string basePath = "./assets/graphics/shaders/hud/";

  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "stackRenderer.glsl.vert")
      .setFragmentFilename(basePath + "stackRenderer.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::stackRendererHud),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::lines)
      .addVbo()
      .setVboAsDynamic()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererWireFramesHud),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererTrianglesHud),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "textRenderer.glsl.vert")
      .setFragmentFilename(basePath + "textRenderer.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_texCoord")
      .addAttribute("a_offset_position")
      .addAttribute("a_offset_texCoord")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_scale")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::textRenderer),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec2f)
      .addVboAttribute("a_vertex_texCoord", Geometry::AttrType::Vec2f)
      .addVbo()
      .setVboAsInstanced()
      .setVboAsDynamic()
      .addVboAttribute("a_offset_position", Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_texCoord", Geometry::AttrType::Vec2f)
      .addVboAttribute("a_offset_color", Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_scale", Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::textRenderer),
      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "postProcess.glsl.vert")
      .setFragmentFilename(basePath + "postProcess.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_colorTexture")
      .addUniform("u_outlineTexture")
      .addUniform("u_invResolution");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::postProcess),
      shaderProgramBuilder.getDefinition());

    // geometryBuilder.reset()
    //   .setShader(*shader)
    //   .setPrimitiveType(Geometry::PrimitiveType::triangles)
    //   .addVbo()
    //   .addVboAttribute("a_vertex_position",Geometry::AttrType::Vec3f)
    //   .addVboAttribute("a_vertex_texCoord",Geometry::AttrType::Vec2f);

    // rManager.createGeometryDefinition(gero::asValue(GeometriesAliases::postProcess),
    //                                   geometryBuilder.getDefinition(), true);
  }
}

} // namespace

void
Context::_initializeGraphicResource() {
  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  auto& rManager = graphic.resourceManager;

  _initializeSceneStructures(rManager);
  _initializeHudStructures(rManager);

  { // font

    rManager.createTexture(
      0, "assets/graphics/textures/ascii_font.png", Texture::Quality::pixelated,
      Texture::Pattern::clamped);

  } // font
}
