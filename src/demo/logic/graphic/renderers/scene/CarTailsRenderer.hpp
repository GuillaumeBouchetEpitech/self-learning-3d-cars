
#pragma once

#include "geronimo/graphic/camera/Camera.hpp"

#include "geronimo/graphic/Geometry.hpp"
#include "geronimo/graphic/ShaderProgram.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <memory>

class CarTailsRenderer {
private:
  std::shared_ptr<gero::graphic::ShaderProgram> _shader;

  gero::graphic::Camera::MatricesData _matricesData;

  struct Geometries {
    struct WheelsTrail {
      std::array<gero::graphic::Geometry, 4> wheels;
    };

    std::array<WheelsTrail, 5> bestNewCarsTrails;

    gero::graphic::Geometry leaderCarTrail;
  } _geometries;

  unsigned int _currentTrailIndex = 0;

public:
  CarTailsRenderer() = default;
  ~CarTailsRenderer() = default;

public:
  void initialise();
  void setMatricesData(const gero::graphic::Camera::MatricesData& matricesData);

public:
  void updateLatestTrail();

public:
  void render();
};
