
#include "SceneStackRenderers.hpp"

#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"

void
SceneStackRenderers::initialize() {

  // const std::string basePath = "./assets/graphics/shaders/scene/";
  const std::string basePath = "./src/application/context/graphics/renderers/scene/scene-stack-renderers/shaders/";

  gero::graphics::ShaderProgramBuilder shaderProgramBuilder;
  shaderProgramBuilder.reset()
    .setVertexFilename(basePath + "stackRenderer.glsl.vert")
    .setFragmentFilename(basePath + "stackRenderer.glsl.frag")
    .addAttribute("a_vertex_position")
    .addAttribute("a_vertex_color")
    .addUniform("u_composedMatrix");

  gero::graphics::GeometryBuilder geometryBuilder;
  geometryBuilder.reset()
    .addVbo()
    .setVboAsDynamic()
    .addVboAttribute("a_vertex_position", gero::graphics::Geometry::AttrType::Vec3f)
    .addVboAttribute("a_vertex_color", gero::graphics::Geometry::AttrType::Vec4f);

  StackRenderers::initialize(shaderProgramBuilder.getDefinition(), geometryBuilder.getDefinition(false));
}
