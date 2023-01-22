
#include "Context.hpp"

#include "graphicIds.hpp"

#include "geronimo/system/asValue.hpp"

#include "geronimo/graphic/GeometryBuilder.hpp"
#include "geronimo/graphic/ShaderProgramBuilder.hpp"

namespace {

void
initialiseSceneStructures(gero::graphic::ResourceManager& rManager) {
  const std::string basePath = "./assets/shaders/scene/";

  gero::graphic::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphic::GeometryBuilder geometryBuilder;

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "stackRenderer.glsl.vert")
      .setFragmentFilename(basePath + "stackRenderer.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::stackRendererScene),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_color", gero::graphic::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::stackRendererWireframesScene),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(
      gero::graphic::Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::stackRendererTrianglesScene),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "particles.glsl.vert")
      .setFragmentFilename(basePath + "particles.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_offsetPosition")
      .addAttribute("a_offsetScale")
      .addAttribute("a_offsetColor")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::particles),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute(
        "a_offsetPosition", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_offsetScale", gero::graphic::Geometry::AttrType::Float)
      .addVboAttribute(
        "a_offsetColor", gero::graphic::Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::particles), geometryBuilder.getDefinition(),
      true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "backGroundTorus.glsl.vert")
      .setFragmentFilename(basePath + "backGroundTorus.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture")
      .addUniform("u_animationCoef");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::backGroundTorus),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_texCoord", gero::graphic::Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::backGroundTorus),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "simpleTexture.glsl.vert")
      .setFragmentFilename(basePath + "simpleTexture.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::simpleTexture),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_texCoord", gero::graphic::Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::simpleTexture),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "chessboardFloor.glsl.vert")
      .setFragmentFilename(basePath + "chessboardFloor.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_normal")
      .addAttribute("a_texCoord")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::chessboardFloor),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_normal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_texCoord", gero::graphic::Geometry::AttrType::Vec2f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::chessboardFloor),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "wireframes.glsl.vert")
      .setFragmentFilename(basePath + "wireframes.glsl.frag")
      .addAttribute("a_position")
      .addUniform("u_composedMatrix")
      .addUniform("u_color");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::wireframes),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::wireframes), geometryBuilder.getDefinition(),
      true);

    geometryBuilder.setPrimitiveType(
      gero::graphic::Geometry::PrimitiveType::line_strip);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::wireframesLineStrip),
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
      gero::asValue(ShaderIds::animatedCircuitWalls),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute(
        "a_vertex_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_color", gero::graphic::Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute(
        "a_vertex_normal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_animatedNormal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_index", gero::graphic::Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::animatedCircuitWalls),
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
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix")
      .addUniform("u_alpha")
      .addUniform("u_lowerLimit")
      .addUniform("u_upperLimit");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::animatedCircuitGround),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute(
        "a_vertex_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_color", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_normal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_animatedNormal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_index", gero::graphic::Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::animatedCircuitGround),
      geometryBuilder.getDefinition(), true);
  }

  {

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "modelsCarWheels.glsl.vert")
      .setFragmentFilename(basePath + "modelsCarWheels.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_offset_transform")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineColor")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::modelsCarWheels),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute(
        "a_vertex_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_color", gero::graphic::Geometry::AttrType::Vec3f)
      .addIgnoredVboAttribute(
        "a_vertex_normal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute(
        "a_offset_transform", gero::graphic::Geometry::AttrType::Mat4f)
      .addVboAttribute(
        "a_offset_color", gero::graphic::Geometry::AttrType::Vec4f)
      .addVboAttribute(
        "a_offset_outlineColor", gero::graphic::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::modelsCarWheels),
      geometryBuilder.getDefinition(), true);
  }

  {

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "modelsCarChassis.glsl.vert")
      .setFragmentFilename(basePath + "modelsCarChassis.glsl.frag")
      .addAttribute("a_vertex_position")
      .addAttribute("a_vertex_color")
      .addAttribute("a_vertex_normal")
      .addAttribute("a_offset_transform")
      .addAttribute("a_offset_color")
      .addAttribute("a_offset_outlineColor")
      .addUniform("u_projectionMatrix")
      .addUniform("u_modelViewMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::modelsCarChassis),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute(
        "a_vertex_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_color", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_vertex_normal", gero::graphic::Geometry::AttrType::Vec3f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute(
        "a_offset_transform", gero::graphic::Geometry::AttrType::Mat4f)
      .addVboAttribute(
        "a_offset_color", gero::graphic::Geometry::AttrType::Vec4f)
      .addVboAttribute(
        "a_offset_outlineColor", gero::graphic::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::modelsCarChassis),
      geometryBuilder.getDefinition(), true);
  }
}

void
initialiseHudStructures(gero::graphic::ResourceManager& rManager) {
  const std::string basePath = "./assets/shaders/hud/";

  gero::graphic::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphic::GeometryBuilder geometryBuilder;

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "stackRenderer.glsl.vert")
      .setFragmentFilename(basePath + "stackRenderer.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_color")
      .addUniform("u_composedMatrix");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::stackRendererHud),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::lines)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute("a_color", gero::graphic::Geometry::AttrType::Vec4f);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::stackRendererWireframesHud),
      geometryBuilder.getDefinition(), true);

    geometryBuilder.setPrimitiveType(
      gero::graphic::Geometry::PrimitiveType::triangles);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::stackRendererTrianglesHud),
      geometryBuilder.getDefinition(), true);
  }

  {
    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "textRenderer.glsl.vert")
      .setFragmentFilename(basePath + "textRenderer.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addAttribute("a_offsetPosition")
      .addAttribute("a_offsetTexCoord")
      .addAttribute("a_offsetColor")
      .addAttribute("a_offsetScale")
      .addUniform("u_composedMatrix")
      .addUniform("u_texture");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::textRenderer),
      shaderProgramBuilder.getDefinition());

    geometryBuilder.reset()
      .setShader(*shader)
      .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
      .addVbo()
      .addVboAttribute("a_position", gero::graphic::Geometry::AttrType::Vec2f)
      .addVboAttribute("a_texCoord", gero::graphic::Geometry::AttrType::Vec2f)
      .addVbo()
      .setVboAsInstanced()
      .addVboAttribute(
        "a_offsetPosition", gero::graphic::Geometry::AttrType::Vec3f)
      .addVboAttribute(
        "a_offsetTexCoord", gero::graphic::Geometry::AttrType::Vec2f)
      .addVboAttribute(
        "a_offsetColor", gero::graphic::Geometry::AttrType::Vec4f)
      .addVboAttribute(
        "a_offsetScale", gero::graphic::Geometry::AttrType::Float);

    rManager.createGeometryDefinition(
      gero::asValue(GeometryIds::textRenderer),
      geometryBuilder.getDefinition(), true);
  }

  //
  //
  //

  {

    shaderProgramBuilder.reset()
      .setVertexFilename(basePath + "postProcess.glsl.vert")
      .setFragmentFilename(basePath + "postProcess.glsl.frag")
      .addAttribute("a_position")
      .addAttribute("a_texCoord")
      .addUniform("u_composedMatrix")
      .addUniform("u_colorTexture")
      .addUniform("u_outlineTexture")
      .addUniform("u_invResolution");

    auto shader = rManager.createShader(
      gero::asValue(ShaderIds::postProcess),
      shaderProgramBuilder.getDefinition());

    // geometryBuilder.reset()
    //   .setShader(*shader)
    //   .setPrimitiveType(gero::graphic::Geometry::PrimitiveType::triangles)
    //   .addVbo()
    //   .addVboAttribute("a_position",
    //   gero::graphic::Geometry::AttrType::Vec3f)
    //   .addVboAttribute("a_texCoord",
    //   gero::graphic::Geometry::AttrType::Vec2f);

    // rManager.createGeometryDefinition(gero::asValue(GeometryIds::postProcess),
    //                                   geometryBuilder.getDefinition(), true);
  }
}

} // namespace

void
Context::initialiseGraphicResource() {
  gero::graphic::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphic::GeometryBuilder geometryBuilder;

  auto& rManager = graphic.resourceManager;

  initialiseSceneStructures(rManager);
  initialiseHudStructures(rManager);

  { // font

    rManager.createTexture(
      0, "assets/textures/ascii_font.png",
      gero::graphic::Texture::Quality::pixelated,
      gero::graphic::Texture::Pattern::clamped);

  } // font
}
