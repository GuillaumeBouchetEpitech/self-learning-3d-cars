
#pragma once

#include "geronimo/graphic/FrameBuffer.hpp"
#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/RenderBuffer.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"
#include "geronimo/graphic/Texture.hpp"

#include "geronimo/graphic/camera/Camera.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <memory>

class PostProcess {
private:
  glm::uvec2 _frameSize = {800, 600};

  gero::graphic::Texture _colorTexture;
  gero::graphic::Texture _outlineTexture;
  gero::graphic::RenderBuffer _depthRenderBuffer;
  gero::graphic::FrameBuffer _frameBuffer;

  std::shared_ptr<gero::graphic::ShaderProgram> _shader = nullptr;
  gero::graphic::Geometry _screenQuad;

  struct Animation {
    float duration = 1.0f;
    float timeLeft = 0.0f;
  } _animation;

  gero::graphic::Camera::MatricesData _matricesData;

public:
  PostProcess() = default;
  ~PostProcess() = default;

public:
  void
  setMatricesData(const gero::graphic::Camera::MatricesData& matricesData);

  void initialise(const glm::uvec2& frameSize);
  void dispose();
  void startRecording();
  void stopRecording();
  void render();
  void resize(const glm::uvec2& frameSize);
  void
  setGeometry(const glm::vec2& pos, const glm::vec2& size, float depth = 0.0f);
};
