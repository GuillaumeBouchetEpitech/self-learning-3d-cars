
#include "ChessBoardFloorRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/asValue.hpp"

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
ChessBoardFloorRenderer::initialize(const glm::vec3& center, const glm::vec3& size) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader =
    resourceManager.getShader(gero::asValue(ShadersAliases::chessboardFloor));

  auto geoDef = resourceManager.getGeometryDefinition(
    gero::asValue(GeometriesAliases::chessboardFloor));
  _geometry.initialize(*_shader, geoDef);
  _geometry.setPrimitiveCount(0);

  {

    const glm::ivec2 size = {512, 512};
    auto pixelsPtr = std::make_unique<uint8_t[]>(size.x * size.y * 4);
    uint8_t* rawPixels = pixelsPtr.get();

    const auto setPixel =
      [&size, rawPixels](int x, int y, uint8_t grey, uint8_t alpha) {

        const uint32_t pixelPos = y * 4 * size.x + x * 4;

        rawPixels[pixelPos + 0] = grey;
        rawPixels[pixelPos + 1] = grey;
        rawPixels[pixelPos + 2] = grey;
        rawPixels[pixelPos + 3] = alpha;
      };

    for (int32_t yy = 0; yy < size.y; ++yy)
      for (int32_t xx = 0; xx < size.x; ++xx) {
        if (
          (xx < size.x * 0.5f && yy < size.y * 0.5f) ||
          (xx > size.x * 0.5f && yy > size.y * 0.5f)) {
          setPixel(xx, yy, 32, 220);
        } else {
          setPixel(xx, yy, 100, 255);
        }
      }

    _texture.allocateBlankRgbaUBytes(size,
      gero::graphics::Texture::Quality::smoothed,
      gero::graphics::Texture::Pattern::repeat,
      rawPixels);
  }

  { // compute chessboard ground

    struct Vertex {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoord;
    };

    const glm::vec3 hSize = glm::ceil(size * 0.55f / 100.55f) * 100.0f;
    const glm::vec2 minPos = { center.x - hSize.x, center.y - hSize.y };
    const glm::vec2 maxPos = { center.x + hSize.x, center.y + hSize.y };
    constexpr float height = -0.1f;

    const glm::vec3 normal = {0, 0, 1};

    const glm::vec3 texCoord = hSize / 100.0f;

    std::array<Vertex, 4> quadVertices{{
      {{maxPos.x, minPos.y, height}, normal, {+texCoord.x, -texCoord.y}},
      {{minPos.x, minPos.y, height}, normal, {-texCoord.x, -texCoord.y}},
      {{maxPos.x, maxPos.y, height}, normal, {+texCoord.x, +texCoord.y}},
      {{minPos.x, maxPos.y, height}, normal, {-texCoord.x, +texCoord.y}},
    }};

    std::array<int, 6> indices{{1, 0, 2, 1, 2, 3}};

    std::vector<Vertex> vertices;
    vertices.reserve(indices.size()); // pre-allocate
    for (int index : indices)
      vertices.push_back(quadVertices.at(index));

    _geometry.allocateBuffer(0, vertices);
    _geometry.setPrimitiveCount(vertices.size());

  } // compute chessboard ground
}

void
ChessBoardFloorRenderer::render(const gero::graphics::Camera& inCamera) {
  if (!_shader)
    D_THROW(std::runtime_error, "shader not setup");

  // hide the floor if the gero::graphics::Camera is looking from beneath it
  GlContext::enable(States::cullFace);

  // transparency friendly
  GlContext::disable(States::depthTest);

  _shader->bind();

  // const glm::vec3 lightPos = inCamera.getEye();

  // D_MYLOG("lightPos " << lightPos);

  const auto& matricesData = inCamera.getMatricesData();
  _shader->setUniform("u_composedMatrix", matricesData.composed);
  _shader->setUniform("u_lightPos", inCamera.getEye());
  _shader->setUniform("u_viewPos", inCamera.getEye());

  _texture.bind();
  _geometry.render();

  GlContext::disable(States::cullFace);
  GlContext::enable(States::depthTest);
}