
#pragma once

#include "geronimo/graphics/FrameBuffer.hpp"
#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/RenderBuffer.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/Texture.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <memory>

class PostProcess {
private:
  glm::uvec2 _frameSize = {800, 600};

  gero::graphics::Texture _colorTexture;
  gero::graphics::Texture _outlineTexture;
  gero::graphics::RenderBuffer _depthRenderBuffer;
  gero::graphics::FrameBuffer _frameBuffer;

  std::shared_ptr<gero::graphics::ShaderProgram> _shader = nullptr;
  gero::graphics::Geometry _screenQuad;

  struct Animation {
    float duration = 1.0f;
    float timeLeft = 0.0f;
  } _animation;

  gero::graphics::Camera::MatricesData _matricesData;

public:
  PostProcess() = default;
  ~PostProcess() = default;

public:
  void setMatricesData(const gero::graphics::Camera::MatricesData& matricesData);

  void initialize(const glm::uvec2& frameSize);
  void dispose();
  void startRecording();
  void stopRecording();
  void render();
  void resize(const glm::uvec2& frameSize);
  void
  setGeometry(const glm::vec2& pos, const glm::vec2& size, float depth = 0.0f);
};
