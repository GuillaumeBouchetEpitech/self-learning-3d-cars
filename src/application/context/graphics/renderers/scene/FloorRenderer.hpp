
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/Texture.hpp"

#include "geronimo/graphics/camera/Camera.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>

class FloorRenderer {
public:
  FloorRenderer() = default;
  ~FloorRenderer() = default;

public:
  void initialise(const glm::vec3& center, const glm::vec3& size);

public:
  void render(const gero::graphics::Camera& inCamera);

private:
  std::shared_ptr<gero::graphics::ShaderProgram> _shader;

  gero::graphics::Texture _texture;
  gero::graphics::Geometry _geometry;
};
