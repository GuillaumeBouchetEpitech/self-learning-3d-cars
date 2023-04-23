
#include "CarData.hpp"


void CarData::SingleTransform::lerp(const CarData::SingleTransform& valA, const CarData::SingleTransform& valB, float coef)
{
  position = valA.position + (valB.position - valA.position) * coef;
  orientation = glm::slerp(valA.orientation, valB.orientation, coef);
}

void CarData::CarTransform::lerp(const CarData::CarTransform& valA, const CarData::CarTransform& valB, float coef)
{
  chassis.lerp(valA.chassis, valB.chassis, coef);
  for (std::size_t ii = 0; ii < wheels.size(); ++ii)
    wheels.at(ii).lerp(valA.wheels.at(ii), valB.wheels.at(ii), coef);
}


void CarData::lerp(const CarData& valA, const CarData& valB, float coef)
{
  liveTransforms.lerp(valA.liveTransforms, valB.liveTransforms, coef);

  life = valA.life + (valB.life - valA.life) * coef;
  fitness = valA.fitness + (valB.fitness - valA.fitness) * coef;

  velocity = valA.velocity + (valB.velocity - valA.velocity) * coef;

  for (std::size_t ii = 0; ii < eyeSensors.size(); ++ii)
  {
    auto& sensor = eyeSensors.at(ii);
    const auto& sensorA = valA.eyeSensors.at(ii);
    const auto& sensorB = valB.eyeSensors.at(ii);

    sensor.near = sensorA.near + (sensorB.near - sensorA.near) * coef;
    sensor.far = sensorA.far + (sensorB.far - sensorA.far) * coef;
    sensor.value = sensorA.value + (sensorB.value - sensorA.value) * coef;
  }

  groundSensor.near = valA.groundSensor.near + (valB.groundSensor.near - valA.groundSensor.near) * coef;
  groundSensor.far = valA.groundSensor.far + (valB.groundSensor.far - valA.groundSensor.far) * coef;
  groundSensor.value = valA.groundSensor.value + (valB.groundSensor.value - valA.groundSensor.value) * coef;

  for (std::size_t ii = 0; ii < neuronsValues.size(); ++ii)
  {
    auto& neuronValue = neuronsValues.at(ii);
    const auto& neuronValueA = valA.neuronsValues.at(ii);
    const auto& neuronValueB = valB.neuronsValues.at(ii);

    neuronValue = neuronValueA + (neuronValueB - neuronValueA) * coef;
  }

}
