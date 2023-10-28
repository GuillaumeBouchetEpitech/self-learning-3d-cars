
#include "BackGroundTorusRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"
#include "geronimo/graphics/make-geometries/MakeGeometries.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/containers/static_heap_grid_array.hpp"
#include "geronimo/system/math/constants.hpp"
#include "geronimo/system/rng/DeterministicRng.hpp"

using namespace gero::graphics;

constexpr float k_ringRadius = 500.0f;
constexpr float k_tubeRadius = 100.0f;

namespace {

struct RealVertex {
  glm::vec3 position;
  glm::vec2 texCoord;
};

void
generateTorusVertices(
  uint32_t ringQuality, uint32_t tubeQuality, float ringRadius, float tubeRadius,
  std::vector<RealVertex>& outVertices) {

  gero::graphics::MakeGeometries::Vertices tmpVertices;
  tmpVertices.reserve(2048);
  gero::graphics::MakeGeometries::makeTorus(tmpVertices, ringQuality, tubeQuality, ringRadius, tubeRadius);

  outVertices.reserve(tmpVertices.size());
  for (int ringIndex = 0; ringIndex + 1 < int(ringQuality); ++ringIndex) {

    const std::size_t indexA1 = ringIndex;
    const std::size_t indexB1 = (ringIndex + 1);

    const float texCoordX1 = float(indexA1) / float(ringQuality) * 4.0f;
    const float texCoordX2 = float(indexB1) / float(ringQuality) * 4.0f;

    for (int tubeIndex = 0; tubeIndex < int(tubeQuality); ++tubeIndex) {

      const std::size_t indexA2 = tubeIndex;
      const std::size_t indexB2 = (tubeIndex + 1);

      const float texCoordY1 = float(indexA2) / float(tubeQuality) * 2.0f;
      const float texCoordY2 = float(indexB2) / float(tubeQuality) * 2.0f;

      const int baseIndex = ringIndex * int(tubeQuality) * 6 + tubeIndex * 6;

      const glm::vec3& pos0 = tmpVertices[baseIndex + 0].position;
      const glm::vec3& pos1 = tmpVertices[baseIndex + 1].position;
      const glm::vec3& pos2 = tmpVertices[baseIndex + 2].position;

      const glm::vec3& pos3 = tmpVertices[baseIndex + 3].position;
      const glm::vec3& pos4 = tmpVertices[baseIndex + 4].position;
      const glm::vec3& pos5 = tmpVertices[baseIndex + 5].position;

      outVertices.push_back({pos0, {texCoordX1, texCoordY1}});
      outVertices.push_back({pos1, {texCoordX2, texCoordY2}});
      outVertices.push_back({pos2, {texCoordX1, texCoordY2}});

      outVertices.push_back({pos3, {texCoordX1, texCoordY1}});
      outVertices.push_back({pos4, {texCoordX2, texCoordY2}});
      outVertices.push_back({pos5, {texCoordX2, texCoordY1}});
    }
  }
}
} // namespace

void
BackGroundTorusRenderer::initialize() {

  gero::graphics::ShaderProgramBuilder shaderProgramBuilder;
  gero::graphics::GeometryBuilder geometryBuilder;

  // const std::string basePath = "./assets/graphics/shaders/scene/";
  const std::string basePath = "./src/application/context/graphics/renderers/scene/background-torus/shaders/";

  shaderProgramBuilder.reset()
    .setVertexFilename(basePath + "backGroundTorus.glsl.vert")
    .setFragmentFilename(basePath + "backGroundTorus.glsl.frag")
    .addAttribute("a_vertex_position")
    .addAttribute("a_vertex_texCoord")
    .addUniform("u_composedMatrix")
    .addUniform("u_texture")
    .addUniform("u_animationCoef");

  _shader = gero::graphics::ShaderProgram::buildUnbound(shaderProgramBuilder.getDefinition());

  geometryBuilder.reset()
    .setShader(*_shader)
    .setPrimitiveType(Geometry::PrimitiveType::triangles)
    .addVbo()
    .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
    .addVboAttribute("a_vertex_texCoord", Geometry::AttrType::Vec2f);

  { // generate the gero::graphics::Texture

    constexpr gero::graphics::Texture::Quality quality = gero::graphics::Texture::Quality::smoothedAndMipMapped;
    constexpr gero::graphics::Texture::Pattern pattern = gero::graphics::Texture::Pattern::repeat;

    const glm::ivec2 texSize = {32, 32};
    auto pixelsPtr = std::make_unique<unsigned char[]>(texSize.x * texSize.y * 4);
    unsigned char* rawPixels = pixelsPtr.get();

    gero::rng::DeterministicRng rng;
    rng.setSeed(0);

    for (int yy = 0; yy < texSize.y; ++yy)
      for (int xx = 0; xx < texSize.x; ++xx) {

        const int texelIndex = yy * 4 * texSize.x + xx * 4;

        const int color = rng.getRangedValue(16, 64);
        rawPixels[texelIndex + 0] = color;
        rawPixels[texelIndex + 1] = color;
        rawPixels[texelIndex + 2] = color;
        rawPixels[texelIndex + 3] = 255;
      }

    _texture.allocateBlankRgbaUBytes(texSize, quality, pattern, rawPixels);

  } // generate the gero::graphics::Texture

  {

    std::vector<RealVertex> vertices;
    vertices.reserve(2048); // pre-allocate

    constexpr uint32_t l_ringQuality = 128;
    constexpr uint32_t k_tubeQuality = 64;

    generateTorusVertices(l_ringQuality, k_tubeQuality, k_ringRadius, k_tubeRadius, vertices);

    _geometry.initialize(*_shader, geometryBuilder.getDefinition());
    _geometry.allocateBuffer(0, vertices);
    _geometry.setPrimitiveCount(vertices.size());
  }
}

void
BackGroundTorusRenderer::setCamera(const gero::graphics::ICamera* inCamera) {
  _camera = inCamera;
}

void
BackGroundTorusRenderer::update(float elapsedTime) {
  _animationTime += elapsedTime * 0.01f;
  while (_animationTime > 1.0f)
    _animationTime -= 1.0f;
}

void
BackGroundTorusRenderer::render() {

  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");
  if (!_camera)
    D_THROW(std::runtime_error, "camera not setup");

  _shader->preBind([this](gero::graphics::IBoundShaderProgram& bound) {
    const glm::vec2 vSize = glm::vec2(Context::get().graphic.renderer.getHudRenderer().getCamera().getSize());

    gero::graphics::Camera tmpCamera;
    tmpCamera.setSize(vSize.x, vSize.y);
    tmpCamera.setPerspective(70.0f, 95.0f, 6000.0f);
    tmpCamera.lookAt(_camera->getEye(), _camera->getTarget(), _camera->getUp());
    tmpCamera.computeMatrices();

    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, _camera->getTarget() + glm::vec3(0, k_ringRadius * 1.0f, 0));
    model = glm::rotate(model, gero::math::hpi, glm::vec3(0, 0, 1));
    model = glm::scale(model, glm::vec3(1, 2, 1));

    const glm::mat4 composed = tmpCamera.getMatricesData().composed * model;

    bound.setUniform("u_composedMatrix", composed);
    bound.setUniform("u_animationCoef", _animationTime);

    _texture.bind();
    _geometry.render();
  });
}
