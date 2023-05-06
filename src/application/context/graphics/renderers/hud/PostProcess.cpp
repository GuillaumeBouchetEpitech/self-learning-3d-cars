
#include "PostProcess.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/system/asValue.hpp"

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
PostProcess::setMatricesData(
  const gero::graphics::Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void
PostProcess::initialize(const glm::uvec2& frameSize) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader =
    resourceManager.getShader(gero::asValue(ShadersAliases::postProcess));

  auto geoDef =
    // resourceManager.getGeometryDefinition(gero::asValue(GeometriesAliases::postProcess));
    resourceManager.getGeometryDefinition(
      gero::asValue(GeometriesAliases::simpleTexture));
  _screenQuad.initialize(*_shader, geoDef);
  _screenQuad.setPrimitiveCount(0);

  resize(frameSize);
}

void
PostProcess::startRecording() {
  _frameBuffer.bind();

  GlContext::setViewport(0, 0, _frameSize.x, _frameSize.y);
  GlContext::clearColor(0, 0, 0, 0);
  GlContext::clearDepth(1.0f);
  GlContext::clears(Buffers::color, Buffers::depth);

  GlContext::setDepthFunc(DepthFuncs::less);
}

void
PostProcess::stopRecording() {
  gero::graphics::FrameBuffer::unbind();
  GlContext::setViewport(0, 0, _frameSize.x, _frameSize.y);
}

void
PostProcess::render() {

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);
  _shader->setUniform(
    "u_invResolution", 1.0f / float(_frameSize.x), 1.0f / float(_frameSize.y));

  _shader->setUniform("u_colorTexture", 0);
  GlContext::Texture::active(0);
  _colorTexture.bind();

  _shader->setUniform("u_outlineTexture", 1);
  GlContext::Texture::active(1);
  _outlineTexture.bind();

  _screenQuad.render();

  GlContext::Texture::active(0);
}

void
PostProcess::resize(const glm::uvec2& inFrameSize) {

  _frameSize = inFrameSize;

  {

    _colorTexture.allocateBlankRgbaUBytes(
      _frameSize, gero::graphics::Texture::Quality::pixelated,
      gero::graphics::Texture::Pattern::clamped);
    _outlineTexture.allocateBlankRgbaUBytes(
      _frameSize, gero::graphics::Texture::Quality::pixelated,
      gero::graphics::Texture::Pattern::clamped);
    _depthRenderBuffer.allocateCompatibleDepth(_frameSize);

    gero::graphics::FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &_colorTexture});
    def.colorTextures.push_back({1, &_outlineTexture});
    def.renderBuffer = &_depthRenderBuffer;
    _frameBuffer.initialize(def);
  }
}

void
PostProcess::setGeometry(
  const glm::vec2& inPos, const glm::vec2& inSize, float depth /*= 0.0f*/) {
  struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
  };

  std::array<Vertex, 4> quadVertices{
    {{{inPos.x + inSize.x * 0.0f, inPos.y + inSize.y * 0.0f, depth}, {0, 0}},
     {{inPos.x + inSize.x * 1.0f, inPos.y + inSize.y * 0.0f, depth}, {1, 0}},
     {{inPos.x + inSize.x * 1.0f, inPos.y + inSize.y * 1.0f, depth}, {1, 1}},
     {{inPos.x + inSize.x * 0.0f, inPos.y + inSize.y * 1.0f, depth}, {0, 1}}}};

  std::array<std::size_t, 6> indices{{1, 0, 2, 0, 3, 2}};

  std::array<Vertex, 6> vertices;
  for (std::size_t index = 0; index < indices.size(); ++index)
    vertices.at(index) = quadVertices.at(indices.at(index));

  _screenQuad.updateOrAllocateBuffer(
    0, uint32_t(vertices.size() * sizeof(Vertex)), vertices.data());
  _screenQuad.setPrimitiveCount(uint32_t(vertices.size()));
}
