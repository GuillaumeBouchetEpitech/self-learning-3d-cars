
#pragma once

#include "application/context/graphics/renderers/hud/PostProcess.hpp"

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/Timer.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"

class ThirdPersonCamera {
private:
  glm::vec2 _position;
  glm::vec2 _size;

  bool _isVisible = false;
  gero::Timer _timer;
  gero::easing::GenericEasing<2> _moveEasing;

  PostProcess _postProcess;

  glm::vec3 _eye = {0.0f, 0.0f, 0.0f};
  glm::vec3 _upAxis = {0.0f, 0.0f, 1.0f};
  gero::graphics::Camera _camera;

public:
  ThirdPersonCamera() = default;
  ~ThirdPersonCamera() = default;

public:
  void initialise();

public:
  bool canActivate() const;

public:
  void fadeIn(float delay, float duration);
  void fadeOut(float delay, float duration);
  void resize();

  void update(float elapsedTime);

  void render();
};
