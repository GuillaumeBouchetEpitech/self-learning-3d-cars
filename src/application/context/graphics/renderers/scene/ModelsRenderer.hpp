
#pragma once

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"
#include "geronimo/graphics/camera/Camera.hpp"
#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/Timer.hpp"

#include <array>
#include <memory>
#include <vector>

class ModelsRenderer {
private:
  struct RenderPass {
    std::shared_ptr<gero::graphics::ShaderProgram> shader;
    gero::graphics::Geometry geometry;
  };

  RenderPass _chassis;
  RenderPass _wheels;

  struct Attributes {
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    glm::vec4 color;
    float outlineValue = 1.0f;

    Attributes(
      const glm::vec3& position, const glm::quat& orientation,
      const glm::vec3& scale, const glm::vec4& color)
      : position(position), orientation(orientation), scale(scale),
        color(color) {}
  };

  std::vector<Attributes> _modelsCarChassisMatrices;
  std::vector<Attributes> _modelsCarWheelsMatrices;

  gero::Timer _timer;
  float _colorAlpha = 0.0f;
  float _outlineColoralpha = 0.0f;

public:
  ModelsRenderer() = default;

public:
  void initialize();

public:
  void fadeIn(float delay, float duration);
  void update(float elapsedTime);
  void render(const gero::graphics::Camera& inCamera);
};
