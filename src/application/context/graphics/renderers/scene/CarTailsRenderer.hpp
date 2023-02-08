
#pragma once

#include "geronimo/graphics/camera/Camera.hpp"

#include "geronimo/graphics/Geometry.hpp"
#include "geronimo/graphics/ShaderProgram.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <memory>

class CarTailsRenderer {
private:
  std::shared_ptr<gero::graphics::ShaderProgram> _shader;

  gero::graphics::Camera::MatricesData _matricesData;

  struct Geometries {
    struct WheelsTrail {
      std::array<gero::graphics::Geometry, 4> wheels;
    };

    std::array<WheelsTrail, 5> bestNewCarsTrails;

    gero::graphics::Geometry leaderCarTrail;
  } _geometries;

  unsigned int _currentTrailIndex = 0;

public:
  CarTailsRenderer() = default;
  ~CarTailsRenderer() = default;

public:
  void initialise();
  void setMatricesData(const gero::graphics::Camera::MatricesData& matricesData);

public:
  void updateLatestTrail();

public:
  void render();
};
