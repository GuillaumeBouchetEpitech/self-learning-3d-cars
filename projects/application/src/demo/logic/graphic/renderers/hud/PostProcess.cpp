
#include "PostProcess.hpp"

#include "demo/logic/Context.hpp"
#include "demo/logic/graphicIds.hpp"

#include "geronimo/graphic/GlContext.hpp"
#include "geronimo/system/asValue.hpp"

void
PostProcess::setMatricesData(
  const gero::graphic::Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void
PostProcess::initialise(const glm::uvec2& frameSize) {

  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(gero::asValue(ShaderIds::postProcess));

  auto geoDef =
    // resourceManager.getGeometryDefinition(gero::asValue(GeometryIds::postProcess));
    resourceManager.getGeometryDefinition(
      gero::asValue(GeometryIds::simpleTexture));
  _screenQuad.initialise(*_shader, geoDef);
  _screenQuad.setPrimitiveCount(0);

  resize(frameSize);
}

void
PostProcess::startRecording() {
  _frameBuffer.bind();

  gero::graphic::GlContext::setViewport(0, 0, _frameSize.x, _frameSize.y);
  gero::graphic::GlContext::clearColor(0, 0, 0, 0);
  gero::graphic::GlContext::clearDepth(1.0f);
  gero::graphic::GlContext::clear(
    gero::asValue(gero::graphic::GlContext::Buffers::color) |
    gero::asValue(gero::graphic::GlContext::Buffers::depth));

  gero::graphic::GlContext::setDepthFunc(
    gero::graphic::GlContext::DepthFuncs::less);
}

void
PostProcess::stopRecording() {
  gero::graphic::FrameBuffer::unbind();
  gero::graphic::GlContext::setViewport(0, 0, _frameSize.x, _frameSize.y);
}

void
PostProcess::render() {

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);
  _shader->setUniform(
    "u_invResolution", 1.0f / float(_frameSize.x), 1.0f / float(_frameSize.y));

  _shader->setUniform("u_colorTexture", 0);
  gero::graphic::GlContext::Texture::active(0);
  _colorTexture.bind();

  _shader->setUniform("u_outlineTexture", 1);
  gero::graphic::GlContext::Texture::active(1);
  _outlineTexture.bind();

  _screenQuad.render();

  gero::graphic::GlContext::Texture::active(0);
}

void
PostProcess::resize(const glm::uvec2& inFrameSize) {

  _frameSize = inFrameSize;

  {

    _colorTexture.allocateBlank(
      _frameSize, gero::graphic::Texture::Quality::pixelated,
      gero::graphic::Texture::Pattern::clamped);
    _outlineTexture.allocateBlank(
      _frameSize, gero::graphic::Texture::Quality::pixelated,
      gero::graphic::Texture::Pattern::clamped);
    _depthRenderBuffer.allocateCompatibleDepth(_frameSize);

    gero::graphic::FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &_colorTexture});
    def.colorTextures.push_back({1, &_outlineTexture});
    def.renderBuffer = &_depthRenderBuffer;
    _frameBuffer.initialise(def);
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
  // std::array<std::size_t, 3> indices{{ 0,3,2 }};

  std::array<Vertex, 6> vertices;
  for (std::size_t index = 0; index < indices.size(); ++index)
    vertices.at(index) = quadVertices.at(indices.at(index));

  _screenQuad.updateBuffer(
    0, vertices.data(), uint32_t(vertices.size() * sizeof(Vertex)));
  _screenQuad.setPrimitiveCount(uint32_t(vertices.size()));
}
