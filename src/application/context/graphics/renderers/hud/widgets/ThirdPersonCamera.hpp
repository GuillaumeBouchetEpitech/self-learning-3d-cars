
#pragma once

#include "application/context/graphics/renderers/hud/post-process/PostProcess.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

// TODO: smoother camera movements
class ThirdPersonCamera {
private:
  struct Layout {
    glm::vec2 position;
    glm::vec2 size;

    bool isVisible = false;
    gero::Timer timer;
    gero::easing::GenericEasing<2> moveEasing;
  } _layout;

  PostProcess _postProcess;

  glm::vec3 _eye = {1.0f, 0.0f, 0.0f};
  glm::vec3 _target = {0.0f, 0.0f, 0.0f};
  glm::vec3 _upAxis = {0.0f, 0.0f, 1.0f};
  gero::graphics::Camera _camera;

public:
  ThirdPersonCamera() = default;
  ~ThirdPersonCamera() = default;

public:
  void initialize();

public:
  bool canActivate() const;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void render();
};
