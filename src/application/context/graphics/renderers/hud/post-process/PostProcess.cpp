
#include "PostProcess.hpp"

#include "application/context/Context.hpp"
#include "geronimo/graphics/GeometryBuilder.hpp"
#include "geronimo/graphics/ShaderProgramBuilder.hpp"

#include "geronimo/graphics/GlContext.hpp"
#include "geronimo/system/asValue.hpp"

using namespace gero::graphics;
using namespace gero::graphics::GlContext;

void
PostProcess::setMatricesData(const gero::graphics::Camera::MatricesData& matricesData) {
  _matricesData = matricesData;
}

void
PostProcess::initialize(uint32_t width, uint32_t height) {

  // const std::string basePath = "./assets/graphics/shaders/hud/";
  const std::string basePath = "./src/application/context/graphics/renderers/hud/post-process/shaders/";

  ShaderProgramBuilder shaderProgramBuilder;
  GeometryBuilder geometryBuilder;

  shaderProgramBuilder.reset()
    .setVertexFilename(basePath + "postProcess.glsl.vert")
    .setFragmentFilename(basePath + "postProcess.glsl.frag")
    .addAttribute("a_vertex_position")
    .addAttribute("a_vertex_texCoord")
    .addUniform("u_composedMatrix")
    .addUniform("u_colorTexture")
    .addUniform("u_outlineTexture")
    .addUniform("u_invResolution");

  _shader = std::make_shared<ShaderProgram>(shaderProgramBuilder.getDefinition());

  geometryBuilder.reset()
    .setShader(*_shader)
    .setPrimitiveType(Geometry::PrimitiveType::triangles)
    .addVbo()
    .addVboAttribute("a_vertex_position", Geometry::AttrType::Vec3f)
    .addVboAttribute("a_vertex_texCoord", Geometry::AttrType::Vec2f);

  _screenQuad.initialize(*_shader, geometryBuilder.getDefinition());
  _screenQuad.setPrimitiveCount(0);

  resize(width, height);
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

  _shader->preBind([this](gero::graphics::IBoundShaderProgram& bound)
  {
    bound.setUniform("u_composedMatrix", _matricesData.composed);
    bound.setUniform("u_invResolution", 1.0f / float(_frameSize.x), 1.0f / float(_frameSize.y));

    bound.setUniform("u_colorTexture", 0);
    GlContext::Texture::active(0);
    _colorTexture.bind();

    bound.setUniform("u_outlineTexture", 1);
    GlContext::Texture::active(1);
    _outlineTexture.bind();

    _screenQuad.render();
  });

  GlContext::Texture::active(0);
}

void
PostProcess::resize(uint32_t width, uint32_t height) {

  _frameSize.x = width;
  _frameSize.y = height;

  {

    _colorTexture.allocateBlankRgbaUBytes(
      _frameSize, gero::graphics::Texture::Quality::pixelated, gero::graphics::Texture::Pattern::clamped);
    _outlineTexture.allocateBlankRgbaUBytes(
      _frameSize, gero::graphics::Texture::Quality::pixelated, gero::graphics::Texture::Pattern::clamped);
    _depthRenderBuffer.allocateCompatibleDepth(_frameSize);

    gero::graphics::FrameBuffer::Definition def;
    def.colorTextures.push_back({0, &_colorTexture});
    def.colorTextures.push_back({1, &_outlineTexture});
    def.renderBuffer = &_depthRenderBuffer;
    _frameBuffer.initialize(def);
  }
}

void
PostProcess::setGeometry(const glm::vec2& inPos, const glm::vec2& inSize, float depth /*= 0.0f*/) {
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

  _screenQuad.updateOrAllocateBuffer(0, uint32_t(vertices.size() * sizeof(Vertex)), vertices.data());
  _screenQuad.setPrimitiveCount(uint32_t(vertices.size()));
}
