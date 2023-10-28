
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
public:
  ModelsRenderer() = default;

public:
  void initialize();

public:
  void setCamera(const gero::graphics::ICamera* inCamera);

public:
  void fadeIn(float delay, float duration);
  void update(float elapsedTime);
  void render();

private:
  struct RenderPass {
    std::shared_ptr<gero::graphics::IUnboundShaderProgram> shader;
    gero::graphics::Geometry geometry;
  };

  RenderPass _chassis;
  RenderPass _wheels;

  struct Attributes {
    glm::vec3 position;
    glm::quat orientation;
    glm::vec3 scale;
    glm::vec3 color;
    float life;

    Attributes(
      const glm::vec3& position, const glm::quat& orientation, const glm::vec3& scale, const glm::vec3& color,
      float life)
      : position(position), orientation(orientation), scale(scale), color(color), life(life) {}
  };

  std::vector<Attributes> _modelsCarChassisMatrices;
  std::vector<Attributes> _modelsCarWheelsMatrices;

  gero::Timer _timer;
  float _colorAlpha = 0.0f;
  float _outlineColoralpha = 0.0f;

  const gero::graphics::ICamera* _camera = nullptr;

};
