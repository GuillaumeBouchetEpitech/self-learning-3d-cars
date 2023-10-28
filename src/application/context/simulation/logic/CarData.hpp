
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/messaging/MessageBuffer.hpp"
#include "geronimo/system/messaging/MessageView.hpp"

#include <array>
#include <vector>

struct CarData {

public:
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

public:
  CarData();

  void lerp(const CarData& valA, const CarData& valB, float coef);

  // void serialize(gero::messaging::MessageBuffer& inMessage) const;
  // void deserialize(gero::messaging::MessageView& inMessage) const;

public:
  bool isAlive = false;
  bool isDying = false;
  float life = 0.0f;
  float fitness = 0.0f;
  uint32_t totalUpdates = 0;
  int32_t groundIndex = -1;

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
