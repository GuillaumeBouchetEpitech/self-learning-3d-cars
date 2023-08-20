
#include "CarData.hpp"

#include "geronimo/system/math/clamp.hpp"
#include "geronimo/system/math/lerp.hpp"

void
CarData::SingleTransform::lerp(const CarData::SingleTransform& valA, const CarData::SingleTransform& valB, float coef) {
  position = gero::math::lerp(valA.position, valB.position, coef);
  orientation = gero::math::lerp(valA.orientation, valB.orientation, coef);
}

void
CarData::CarTransform::lerp(const CarData::CarTransform& valA, const CarData::CarTransform& valB, float coef) {
  chassis.lerp(valA.chassis, valB.chassis, coef);
  for (std::size_t ii = 0; ii < wheels.size(); ++ii)
    wheels.at(ii).lerp(valA.wheels.at(ii), valB.wheels.at(ii), coef);
}

CarData::CarData() {
  latestTransformsHistory.reserve(50); // TODO: hardcoded
}

void
CarData::lerp(const CarData& valA, const CarData& valB, float inCoef) {

  const float coef = gero::math::clamp(inCoef, 0.0f, 1.0f);

  liveTransforms.lerp(valA.liveTransforms, valB.liveTransforms, coef);

  life = gero::math::lerp(valA.life, valB.life, coef);
  life = gero::math::clamp(life, 0.0f, 1.0f);

  fitness = gero::math::lerp(valA.fitness, valB.fitness, coef);
  velocity = gero::math::lerp(valA.velocity, valB.velocity, coef);

  for (std::size_t ii = 0; ii < eyeSensors.size(); ++ii) {
    auto& sensor = eyeSensors.at(ii);
    const auto& sensorA = valA.eyeSensors.at(ii);
    const auto& sensorB = valB.eyeSensors.at(ii);

    sensor.near = gero::math::lerp(sensorA.near, sensorB.near, coef);
    sensor.far = gero::math::lerp(sensorA.far, sensorB.far, coef);
    sensor.value = gero::math::lerp(sensorA.value, sensorB.value, coef);
  }

  groundSensor.near = gero::math::lerp(valA.groundSensor.near, valB.groundSensor.near, coef);
  groundSensor.far = gero::math::lerp(valA.groundSensor.far, valB.groundSensor.far, coef);
  groundSensor.value = gero::math::lerp(valA.groundSensor.value, valB.groundSensor.value, coef);

  for (std::size_t ii = 0; ii < neuronsValues.size(); ++ii) {
    auto& neuronValue = neuronsValues.at(ii);
    const auto& neuronValueA = valA.neuronsValues.at(ii);
    const auto& neuronValueB = valB.neuronsValues.at(ii);

    neuronValue = gero::math::lerp(neuronValueA, neuronValueB, coef);
  }
}

// void CarData::serialize(gero::messaging::MessageBuffer& inMessage) const
// {

// }
// void CarData::deserialize(gero::messaging::MessageView& inMessage) const
// {

// }
