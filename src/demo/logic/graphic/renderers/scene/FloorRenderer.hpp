
#pragma once

#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"
#include "geronimo/graphic/Texture.hpp"

#include "geronimo/graphic/camera/Camera.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <memory>
#include <vector>

class FloorRenderer {
public:
  FloorRenderer() = default;
  ~FloorRenderer() = default;

public:
  void initialise(const glm::vec3& center, const glm::vec3& size);
  void setMatricesData(const gero::graphic::Camera::MatricesData& matricesData);

public:
  void render();

private:
  std::shared_ptr<gero::graphic::ShaderProgram> _shader;

  gero::graphic::Texture _texture;
  gero::graphic::Geometry _geometry;

  gero::graphic::Camera::MatricesData _matricesData;
};
