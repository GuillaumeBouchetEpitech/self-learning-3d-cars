
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/Texture.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

#include <array>
#include <memory>

class BackGroundTorusRenderer {
public:
  BackGroundTorusRenderer() = default;
  ~BackGroundTorusRenderer() = default;

public:
  void initialize();

public:
  void setCamera(const gero::graphics::ICamera* inCamera);

public:
  void update(float elapsedTime);
  void render();

private:
  std::shared_ptr<gero::graphics::IUnboundShaderProgram> _shader;
  gero::graphics::Geometry _geometry;
  gero::graphics::Texture _texture;

  float _animationTime = 0.0f;

  const gero::graphics::ICamera* _camera = nullptr;
};
