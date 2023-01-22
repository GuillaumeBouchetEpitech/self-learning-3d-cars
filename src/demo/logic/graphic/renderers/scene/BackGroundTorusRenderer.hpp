
#pragma once

#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"
#include "geronimo/graphic/Texture.hpp"

#include "geronimo/graphic/camera/Camera.hpp"

#include <array>
#include <memory>

class BackGroundTorusRenderer {
public:
  BackGroundTorusRenderer() = default;
  ~BackGroundTorusRenderer() = default;

public:
  void initialise();

public:
  void update(float elapsedTime);
  void render(const gero::graphic::Camera& inCamera);

private:
  std::shared_ptr<gero::graphic::ShaderProgram> _shader;
  gero::graphic::Geometry _geometry;
  gero::graphic::Texture _texture;

  float _animationTime = 0.0f;
};
