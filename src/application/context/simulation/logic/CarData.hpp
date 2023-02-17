
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <vector>

struct CarData {
  bool isAlive = false;
  float life = 0.0f;
  float fitness = 0.0f;
  unsigned int totalUpdates = 0;
  int groundIndex = -1;

  struct SingleTransform {
    glm::vec3 position;
    glm::quat orientation;
  };
  struct CarTransform {
    SingleTransform chassis;
    std::array<SingleTransform, 4> wheels;
  };
  CarTransform liveTransforms;

  std::vector<CarTransform> latestTransformsHistory;

  glm::vec3 velocity = {0, 0, 0};

  struct SensorData {
    glm::vec3 near = {0, 0, 0};
    glm::vec3 far = {0, 0, 0};
    float value = 0.0f;
  };

  std::array<SensorData, 15> eyeSensors;
  SensorData groundSensor;

  std::vector<float> neuronsValues;
};

using AllCarsData = std::vector<CarData>;
