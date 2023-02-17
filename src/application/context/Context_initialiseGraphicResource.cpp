
#include "Context.hpp"

#include "graphics/graphicsAliases.hpp"

#include "geronimo/system/asValue.hpp"

#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"

namespace {

void _initialiseSceneStructures(gero::graphics::ResourceManager& rManager)
{
  const std::string basePath = "./assets/graphics/shaders/scene/";

  gero::graphics::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphics::GeometryBuilder geometryBuilder;

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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", gero::graphics::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererWireFramesScene),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(
      gero::graphics::Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererTrianglesScene),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "particles.glsl.vert")
      .setFragmentFilename(basePath + "particles.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_offset_position")
      .addAttribute("a_offset_scale")
      .addAttribute("a_offset_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::particles),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute(
        "a_offset_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_offset_scale", gero::graphics::Geometry::AttrType::Float)
      .addVboAttribute(
        "a_offset_color", gero::graphics::Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::particles), geometryBuilder.getDefinition(),
      true);
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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_texCoord", gero::graphics::Geometry::AttrType::Vec2f);

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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_texCoord", gero::graphics::Geometry::AttrType::Vec2f);

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
      ;

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::chessboardFloor),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_texCoord", gero::graphics::Geometry::AttrType::Vec2f);

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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::wireFrames), geometryBuilder.getDefinition(),
      true);

    geometryBuilder.setPrimitiveType(
      gero::graphics::Geometry::PrimitiveType::line_strip);

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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute(
        "a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_color", gero::graphics::Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute(
        "a_vertex_normal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_animatedNormal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_index", gero::graphics::Geometry::AttrType::Float);

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
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::animatedCircuitGround),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute(
        "a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_color", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_normal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_animatedNormal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_index", gero::graphics::Geometry::AttrType::Float);

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
      .addAttribute("a_offset_outlineColor")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::modelsCarWheels),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", gero::graphics::Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute("a_vertex_normal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offset_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_orientation", gero::graphics::Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_scale", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_color", gero::graphics::Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineColor", gero::graphics::Geometry::AttrType::Vec4f);

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
      .addAttribute("a_offset_outlineColor")
      .addUniform("u_composedMatrix")
      .addUniform("u_lightPos")
      ;

    auto shader = rManager.createShader(
      gero::asValue(ShadersAliases::modelsCarChassis),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_normal", gero::graphics::Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute("a_offset_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_orientation", gero::graphics::Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_scale", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_offset_color", gero::graphics::Geometry::AttrType::Vec4f)
      .addVboAttribute("a_offset_outlineColor", gero::graphics::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::modelsCarChassis),
      geometryBuilder.getDefinition(), true);
  }
}

void
_initialiseHudStructures(gero::graphics::ResourceManager& rManager) {
  const std::string basePath = "./assets/graphics/shaders/hud/";

  gero::graphics::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphics::GeometryBuilder geometryBuilder;

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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_vertex_color", gero::graphics::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::stackRendererWireFramesHud),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(
      gero::graphics::Geometry::PrimitiveType::triangles);

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
      .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec2f)
      .addVboAttribute("a_vertex_texCoord", gero::graphics::Geometry::AttrType::Vec2f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute(
        "a_offset_position", gero::graphics::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_offset_texCoord", gero::graphics::Geometry::AttrType::Vec2f)
      .addVboAttribute(
        "a_offset_color", gero::graphics::Geometry::AttrType::Vec4f)
      .addVboAttribute(
        "a_offset_scale", gero::graphics::Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometriesAliases::textRenderer), geometryBuilder.getDefinition(),
      true);
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
    //   .setPrimitiveType(gero::graphics::Geometry::PrimitiveType::triangles)
    //   .addVbo()
    //   .addVboAttribute("a_vertex_position",
    //   gero::graphics::Geometry::AttrType::Vec3f)
    //   .addVboAttribute("a_vertex_texCoord",
    //   gero::graphics::Geometry::AttrType::Vec2f);

    // rManager.createGeometryDefinition(gero::asValue(GeometriesAliases::postProcess),
    //                                   geometryBuilder.getDefinition(), true);
  }
}

} // namespace

void
Context::_initialiseGraphicResource() {
  gero::graphics::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphics::GeometryBuilder geometryBuilder;

  auto& rManager = graphic.resourceManager;

  _initialiseSceneStructures(rManager);
  _initialiseHudStructures(rManager);

  { // font

    rManager.createTexture(
      0, "assets/graphics/textures/ascii_font.png",
      gero::graphics::Texture::Quality::pixelated,
      gero::graphics::Texture::Pattern::clamped);

  } // font
}
