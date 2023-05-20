
#pragma once

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <vector>

struct CarData {
  bool isAlive = false;
  bool isDying = false;
  float life = 0.0f;
  float fitness = 0.0f;
  unsigned int totalUpdates = 0;
  int groundIndex = -1;

  struct SingleTransform {
    glm::vec3 position;
    glm::quat orientation;

    void lerp(const SingleTransform& valA, const SingleTransform& valB, float coef);
  };
  struct CarTransform {
    SingleTransform chassis;
    std::array<SingleTransform, 4> wheels;

    void lerp(const CarTransform& valA, const CarTransform& valB, float coef);
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

  CarData();

  void lerp(const CarData& valA, const CarData& valB, float coef);
};

using AllCarsData = std::vector<CarData>;
