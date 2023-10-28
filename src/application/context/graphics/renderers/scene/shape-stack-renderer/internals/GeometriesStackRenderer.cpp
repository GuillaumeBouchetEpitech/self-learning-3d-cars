
#include "GeometriesStackRenderer.hpp"

#include "application/context/Context.hpp"
#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"

using namespace gero::graphics;

void
GeometriesStackRenderer::initialize() {

  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  // const std::string basePath = "./assets/graphics/shaders/scene/";
  const std::string basePath = "./src/application/context/graphics/renderers/scene/shape-stack-renderer/internals/shaders/";

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

  _shader = std::make_shared<ShaderProgram>(shaderProgramBuilder.getDefinition());

  geometryBuilder.reset()
    .setShader(*_shader)
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

  _geoDef = geometryBuilder.getDefinition();
}

void
GeometriesStackRenderer::createAlias(int32_t alias, const gero::graphics::MakeGeometries::Vertices& vertices) {

  auto newAlias = std::make_shared<AliasedGeometry>();

  newAlias->instanceVertices.reserve(256); // pre-allocate

  newAlias->geometry.initialize(*_shader, _geoDef);
  newAlias->geometry.allocateBuffer(0, vertices);
  newAlias->geometry.preAllocateBufferFromCapacity(1, newAlias->instanceVertices);
  newAlias->geometry.setPrimitiveStart(0);
  newAlias->geometry.setPrimitiveCount(uint32_t(vertices.size()));

  _aliasedGeometriesMap[alias] = newAlias;
}

void
GeometriesStackRenderer::deleteAlias(int32_t alias) {
  auto it = _aliasedGeometriesMap.find(alias);
  if (it == _aliasedGeometriesMap.end())
    D_THROW(std::runtime_error, "alias not found: " << alias);

  _aliasedGeometriesMap.erase(it);
}
void
GeometriesStackRenderer::clearAlias(int32_t alias) {
  auto it = _aliasedGeometriesMap.find(alias);
  if (it == _aliasedGeometriesMap.end())
    D_THROW(std::runtime_error, "alias not found: " << alias);

  it->second->instanceVertices.clear();
}

void
GeometriesStackRenderer::pushAlias(
  int32_t alias, const GeometryInstance& newInstance, float inForwardOffset /*= 0.0f*/) {
  auto it = _aliasedGeometriesMap.find(alias);
  if (it == _aliasedGeometriesMap.end())
    D_THROW(std::runtime_error, "alias not found: " << alias);

  GeometryInstance copyInstance = newInstance;

  copyInstance.orientation = copyInstance.orientation;

  if (inForwardOffset != 0.0f) {
    copyInstance.position += glm::mat3_cast(copyInstance.orientation) * glm::vec3(inForwardOffset, 0, 0);
  }

  it->second->instanceVertices.push_back(copyInstance);
}

void
GeometriesStackRenderer::clearAll() {
  _aliasedGeometriesMap.clear();
}

void
GeometriesStackRenderer::renderAll(const gero::graphics::ICamera& inCamera) {
  if (!_shader)
    D_THROW(std::runtime_error, "not initialized");

  if (_aliasedGeometriesMap.empty())
    return;

  _shader->preBind([this, &inCamera](gero::graphics::IBoundShaderProgram& bound)
  {
    bound.setUniform("u_composedMatrix", inCamera.getMatricesData().composed);
    // bound.setUniform("u_ambiantCoef", 0.2f);
    // bound.setUniform("u_lightPos", inLightPos.x, inLightPos.y, inLightPos.z);

    for (const auto& pair : _aliasedGeometriesMap) {
      auto& vertices = pair.second->instanceVertices;
      if (vertices.empty())
        continue;

      // D_MYLOG("step");

      auto& geometry = pair.second->geometry;

      geometry.updateOrAllocateBuffer(1, vertices);
      geometry.setInstancedCount(uint32_t(vertices.size()));
      geometry.render();

      vertices.clear();
    }
  });
}
