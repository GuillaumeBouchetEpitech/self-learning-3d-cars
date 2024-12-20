
#include "CarAgent.hpp"

#include "common.hpp"

#include "geronimo/system/TraceLogger.hpp"
#include "geronimo/system/asValue.hpp"
#include "geronimo/system/easing/GenericEasing.hpp"
#include "geronimo/system/math/constants.hpp"

#include <cmath>
#include <iostream>

namespace constants {

constexpr float steeringMaxValue = gero::math::opi;
// constexpr float speedMaxValue = 10.0f;
// constexpr float speedMaxValue = 30.0f;
constexpr float speedMaxValue = 25.0f;
constexpr float healthMaxValue = 1.5f;

constexpr float eyeMaxRange = 50.0f;
constexpr float eyeHeight = 1.0f;
constexpr float eyeElevation = 6.0f;
constexpr float eyeWidthStep = gero::math::opi;

const std::array<float, 3> eyeElevations = {
  +eyeElevation,
  0.0f,
  -eyeElevation,
};

const std::array<float, 5> eyeAngles = {-eyeWidthStep * 2.0f, -eyeWidthStep, 0.0f, +eyeWidthStep, +eyeWidthStep * 2.0f};

constexpr float groundMaxRange = 10.0f; // <= ground sensor
constexpr float groundHeight = 1.0f;

} // namespace constants

void
CarAgent::update(float elapsedTime, NeuralNetwork& neuralNetwork) {
  if (_health <= 0.0f) {
    return;
  }

  _updateSensors();
  _collideEyeSensors();

  const bool hasHitGround = _collideGroundSensor();

  if (hasHitGround == false) {
    _isDying = true;
  }

  // reduce the health over time
  // => reduce more if the car does not touch the ground
  // => faster discard of a most probably dying genome
  _health -= (_isDying ? 2.0f : 1.0f) * elapsedTime;

  if (_health <= 0.0f) {

    auto& vehicleManager = _physicWorld->getPhysicVehicleManager();
    auto& physicBodyManager = _physicWorld->getPhysicBodyManager();

    if (_physicVehicle)
      vehicleManager.removeVehicle(_physicVehicle);
    if (_physicBody)
      physicBodyManager.removeBody(_physicBody);

    return;
  }

  std::vector<float> input;
  input.reserve(_eyeSensors.size()); // pre-allocate
  for (const auto& eyeSensor : _eyeSensors) {
    // ensure input range is [0..1]
    input.push_back(glm::clamp(eyeSensor.value, 0.0f, 1.0f));
  }

  const float speed = _physicVehicle->getCurrentSpeedKmHour();
  // D_MYLOG("speed=" << speed);
  input.push_back(glm::clamp(speed / 100.0f, 0.0f, 1.0f));

  std::vector<float> output;

  neuralNetwork.compute(input, output);

  // ensure output range is [0..1]
  output.at(0) = glm::clamp(output.at(0), 0.0f, 1.0f);
  output.at(1) = glm::clamp(output.at(1), 0.0f, 1.0f);

  // steering angle: left/right
  _output.steer = output.at(0) * 2.0f - 1.0f; // [-1..+1]
  // speed coef: forward/backward
  _output.speed = output.at(1); // [0..+1]

  const float steerVal = _output.steer * constants::steeringMaxValue;
  _physicVehicle->setSteeringValue(0, steerVal);
  _physicVehicle->setSteeringValue(1, steerVal);

  auto engineEasing = gero::easing::GenericEasing<5>();
  engineEasing.push(0.0f, -constants::speedMaxValue); // reverse
  // [0.000 -> 0.500] -> [-speedMaxValue -> 0.000]
  engineEasing.push(0.5f, 0.0f); // neutral
  // [0.500 -> 0.625] -> [0.000 -> +speedMaxValue * 4]
  engineEasing.push(0.5f + 0.5f * 0.25f, constants::speedMaxValue * 4.0f); // 4 time faster initial acceleration
  // [0.625 -> 0.750] -> [+speedMaxValue * 4 -> +speedMaxValue * 1]
  engineEasing.push(0.5f + 0.5f * 0.50f, constants::speedMaxValue); // linear acceleration
  // [0.750 -> 1.000] -> [+speedMaxValue * 1 -> +speedMaxValue * 1]
  engineEasing.push(0.5f + 0.5f * 1.00f, constants::speedMaxValue);

  const float engineForce = engineEasing.get(_output.speed);

  // _physicVehicle->applyEngineForce(2, engineForce);
  // _physicVehicle->applyEngineForce(3, engineForce);
  _physicVehicle->applyEngineForce(0, engineForce);
  _physicVehicle->applyEngineForce(1, engineForce);

  ++_totalUpdateNumber;
}

void
CarAgent::_createVehicle() {

  auto& vehicleManager = _physicWorld->getPhysicVehicleManager();
  auto& physicBodyManager = _physicWorld->getPhysicBodyManager();

  if (_physicVehicle)
    vehicleManager.destroyVehicle(_physicVehicle);
  if (_physicBody)
    physicBodyManager.destroyBody(_physicBody);

  gero::physics::PhysicBodyDef bodyDef;

  // const glm::vec3 chassisSize = { 1.0f, 2.0f, 0.5f };
  const glm::vec3 chassisSize = {2.0f, 4.0f, 1.0f};
  const glm::vec3 chassisHSize = chassisSize * 0.5f;

  bodyDef.shape.type = gero::physics::PhysicShapeDef::Type::compound;
  {
    gero::physics::PhysicShapeDef::Data::Compound::ChildShape childShape;
    {
      // elevate the chassis
      childShape.transform = glm::identity<glm::mat4>();
      childShape.transform = glm::translate(childShape.transform, glm::vec3(0.0f, 0.0f, 0.9f));
    }
    {
      childShape.shape = std::make_shared<gero::physics::PhysicShapeDef>();
      childShape.shape->type = gero::physics::PhysicShapeDef::Type::box;
      childShape.shape->data.box.size = chassisSize;
    }
    bodyDef.shape.data.compound.childShapes.push_back(childShape);
  }

  bodyDef.mass = 3.0f;
  bodyDef.group = gero::asValue(Groups::vehicle);
  bodyDef.mask = gero::asValue(Groups::ground);

  _physicBody = physicBodyManager.createAndAddBody(bodyDef);
  _physicBody->setPosition({30, 30, 5});
  _physicBody->setFriction(0.0f);
  _physicBody->disableSleep();

  //
  //

  const glm::vec3 wheelDirectionCS0 = {0.0f, 0.0f, -1.0f}; // down axis: -Z (toward the ground)
  const glm::vec3 wheelAxleCS = {1.0f, 0.0f, 0.0f};        // rotation axis: +X
  constexpr float wheelRadius = 0.5f;
  constexpr float wheelWidth = 0.2f;
  constexpr float wheelSide = wheelWidth * 0.3f;

  // The maximum length of the suspension (metres)
  constexpr float suspensionRestLength = 0.5f;

  // The maximum distance the suspension can be compressed (centimetres)
  constexpr float maxSuspensionTravelCm = 40.0f; // <= 0.4 metres

  // The coefficient of friction between the tyre and the ground.
  // Should be about 0.8 for realistic cars, but can increased for better
  // handling. Set large (10000.0) for kart racers
  constexpr float wheelFriction = 10000.0f; // <= "kart racer"

  // The stiffness constant for the suspension.
  // => 10.0: "Off road buggy"
  // => 50.0: "Sports car"
  // => 200.0: "F1 Car"
  constexpr float suspensionStiffness = 100.0f; // <= "Sports/F1 Car"

  // The damping coefficient for when the suspension is compressed.
  // Set to k * 2.0 * btSqrt(m_suspensionStiffness) so k is proportional to
  // critical damping. k = 0.0 undamped & bouncy, k = 1.0 critical damping 0.1
  // to 0.3 are good values
  constexpr float wheelsDampingCompression = 0.3f;

  // The damping coefficient for when the suspension is expanding.
  // See the comments for m_wheelsDampingCompression for how to set k.
  // m_wheelsDampingRelaxation should be slightly larger than
  // m_wheelsDampingCompression, eg 0.2 to 0.5
  constexpr float wheelsDampingRelaxation = 0.5f;

  // Reduces the rolling torque applied from the wheels that cause the vehicle
  // to roll over. This is a bit of a hack, but it's quite effective. 0.0 = no
  // roll, 1.0 = physical behaviour. If m_frictionSlip is too high, you'll need
  // to reduce this to stop the vehicle rolling over. You should also try
  // lowering the vehicle's centre of mass
  constexpr float rollInfluence = 0.0f;

  gero::physics::PhysicVehicleDef vehicleDef;
  vehicleDef.body = _physicBody;
  vehicleDef.coordinateSystem = {0, 2, 1}; // -> X,Z,Y
  vehicleDef.wheelsCollisionGroup = gero::asValue(Groups::vehicle);
  vehicleDef.wheelsCollisionMask = gero::asValue(Groups::ground);
  vehicleDef.allWheelStats.reserve(4);

  struct WheelPosition {
    glm::vec3 connectionPoint;
    bool isFrontWheel;
  };

  std::array<WheelPosition, 4> wheelPositions{{// front right
                                               {{+chassisHSize.x - wheelSide, +chassisHSize.y - wheelRadius, 0.5f},
                                                /*isFrontWheel = */ true},
                                               // front left
                                               {{-chassisHSize.x + wheelSide, +chassisHSize.y - wheelRadius, 0.5f},
                                                /*isFrontWheel = */ true},
                                               // rear right
                                               {{+chassisHSize.x - wheelSide, -chassisHSize.y + wheelRadius, 0.5f},
                                                /*isFrontWheel = */ false},
                                               // rear left
                                               {{-chassisHSize.x + wheelSide, -chassisHSize.y + wheelRadius, 0.5f},
                                                /*isFrontWheel = */ false}}};

  for (auto& wheelPos : wheelPositions) {
    gero::physics::PhysicVehicleDef::WheelStats wheelStats;

    wheelStats.connectionPoint = wheelPos.connectionPoint;
    wheelStats.isFrontWheel = wheelPos.isFrontWheel;
    wheelStats.maxSuspensionTravelCm = maxSuspensionTravelCm;
    wheelStats.rollInfluence = rollInfluence;
    wheelStats.suspensionRestLength = suspensionRestLength;
    wheelStats.suspensionStiffness = suspensionStiffness;
    wheelStats.wheelAxleCS = wheelAxleCS;
    wheelStats.wheelDirectionCS0 = wheelDirectionCS0;
    wheelStats.wheelFriction = wheelFriction;
    wheelStats.wheelRadius = wheelRadius;
    wheelStats.wheelsDampingCompression = wheelsDampingCompression;
    wheelStats.wheelsDampingRelaxation = wheelsDampingRelaxation;

    vehicleDef.allWheelStats.push_back(wheelStats);
  }

  _physicVehicle = vehicleManager.createAndAddVehicle(vehicleDef);
}

void
CarAgent::_updateSensors() {
  glm::mat4 vehicleTransform;
  _physicBody->getTransform(vehicleTransform);

  { // eye sensor

    glm::vec4 newNearValue = vehicleTransform * glm::vec4(0, 0, constants::eyeHeight, 1);

    int32_t sensorIndex = 0;

    for (const auto& eyeElevation : constants::eyeElevations)
      for (const auto& eyeAngle : constants::eyeAngles) {
        CarAgent::Sensor& eyeSensor = _eyeSensors.at(sensorIndex++);

        const glm::vec4 newFarValue(
          constants::eyeMaxRange * std::sin(eyeAngle), constants::eyeMaxRange * std::cos(eyeAngle),
          constants::eyeHeight + eyeElevation, 1.0f);

        eyeSensor.near = newNearValue;
        eyeSensor.far = vehicleTransform * newFarValue;
      }

  } // eye sensor

  { // ground sensor

    _groundSensor.near = vehicleTransform * glm::vec4(0, 0, constants::groundHeight, 1);
    _groundSensor.far = vehicleTransform * glm::vec4(0, 0, constants::groundHeight - constants::groundMaxRange, 1);

  } // ground sensor
}

void
CarAgent::_collideEyeSensors() {
  for (auto& sensor : _eyeSensors) {
    sensor.value = 1.0f;

    // eye sensors collide ground + walls
    gero::physics::RayCaster::RayCastParams params(
      sensor.near, sensor.far, 0, gero::asValue(Groups::sensor), gero::asValue(Masks::eyeSensor));

    gero::physics::RayCaster::RayCastParams::ResultArray<1> result;

    _physicWorld->getRayCaster().rayCast(params, result);

    if (!result.hasHit)
      continue;

    sensor.far = result.allImpactsData.front().impactPoint;

    sensor.value = glm::length(sensor.far - sensor.near) / constants::eyeMaxRange;
  }
}

bool
CarAgent::_collideGroundSensor() {
  // rayCast the ground to get the checkpoints validation

  // ground sensor collide only ground
  gero::physics::RayCaster::RayCastParams params(
    _groundSensor.near, _groundSensor.far, 0, gero::asValue(Groups::sensor), gero::asValue(Masks::groundSensor));

  gero::physics::RayCaster::RayCastParams::ResultArray<1> result;

  _physicWorld->getRayCaster().rayCast(params, result);

  if (!result.hasHit) {
    return false;
  }

  _groundSensor.far = result.allImpactsData.front().impactPoint;

  _groundSensor.value = glm::length(_groundSensor.far - _groundSensor.near) / constants::groundMaxRange;

  if (_isDying == true) {
    return true;
  }

  // int hitGroundIndex = params.result.impactIndex;
  const int32_t hitGroundIndex = result.allImpactsData.front().body->getUserValue();
  // const int hitGroundIndex =
  // result.allImpactsData.front().bodyRef->getUserValue();

  // is this the next "ground geometry" index?
  if (_groundIndex + 1 == hitGroundIndex) {
    // update so it only get rewarded at the next "ground geometry"
    _groundIndex = hitGroundIndex;

    // restore health to full
    _health = constants::healthMaxValue;

    // reward the genome
    ++_fitness;
  }
  // else if (_groundIndex + 1 > hitGroundIndex)
  // {
  //     _physicBody->disableContactResponse();
  // }

  // if (_groundIndex != hitGroundIndex)
  // {
  //     _physicBody->disableContactResponse();
  // }

  return true;
}

void
CarAgent::reset(
  gero::physics::AbstractPhysicWorld* inPhysicWorld, const glm::vec3& position, const glm::vec4& quaternion,
  const glm::vec3& linearVelocity) {
  _fitness = 0;
  _totalUpdateNumber = 0;
  _health = constants::healthMaxValue;
  _groundIndex = 0;
  _isDying = false;

  _output.steer = 0.0f;
  _output.speed = 0.0f;

  _physicWorld = inPhysicWorld;

  _createVehicle();

  // _physicVehicle->reset();
  _physicBody->setPosition(position);
  _physicBody->setOrientation(quaternion);

  _updateSensors();

  _physicVehicle->reset();
  _physicVehicle->getPhysicBody()->applyCentralImpulse(linearVelocity);
}

void
CarAgent::getAsCarData(const NeuralNetwork& inNeuralNet, CarData& inCarData) const {
  // inCarData.latestTransformsHistory.clear();

  inCarData.isAlive = this->isAlive();
  inCarData.isDying = this->isDying();
  inCarData.life = this->getLife();
  inCarData.fitness = this->getFitness();
  inCarData.totalUpdates = this->getTotalUpdates();
  inCarData.groundIndex = this->getGroundIndex();

  if (!inCarData.isAlive) {
    return;
  }

  const auto body = this->getBody();
  const auto vehicle = this->getVehicle();

  // transformation matrix of the car
  inCarData.liveTransforms.chassis.position = body->getPosition();
  inCarData.liveTransforms.chassis.orientation = body->getOrientation();

  // transformation matrix of the wheels
  for (std::size_t jj = 0; jj < inCarData.liveTransforms.wheels.size(); ++jj) {
    inCarData.liveTransforms.wheels.at(jj).position = vehicle->getWheelPosition(jj);
    inCarData.liveTransforms.wheels.at(jj).orientation = vehicle->getWheelOrientation(jj);
  }

  inCarData.velocity = body->getLinearVelocity();

  const auto& eyeSensors = this->getEyeSensors();
  for (std::size_t jj = 0; jj < eyeSensors.size(); ++jj) {

    const auto& inSensor = eyeSensors.at(jj);
    auto& outSensor = inCarData.eyeSensors.at(jj);

    outSensor.near = inSensor.near;
    outSensor.far = inSensor.far;
    outSensor.value = inSensor.value;
  }

  const auto& gSensor = this->getGroundSensor();
  inCarData.groundSensor.near = gSensor.near;
  inCarData.groundSensor.far = gSensor.far;
  inCarData.groundSensor.value = gSensor.value;

  inNeuralNet.getNeuronsValues(inCarData.neuronsValues);
}

bool
CarAgent::isOwnedByPhysicWorld(const gero::physics::AbstractPhysicWorld* inPhysicWorld) const {
  return _physicWorld == inPhysicWorld;
}

const CarAgent::Sensors&
CarAgent::getEyeSensors() const {
  return _eyeSensors;
}

const CarAgent::Sensor&
CarAgent::getGroundSensor() const {
  return _groundSensor;
}

float
CarAgent::getFitness() const {
  return _fitness;
}

bool
CarAgent::isAlive() const {
  return _health > 0.0f;
}

bool
CarAgent::isDying() const {
  return _isDying;
}

int32_t
CarAgent::getGroundIndex() const {
  return _groundIndex;
}

const CarAgent::NeuralNetworkOutput&
CarAgent::getNeuralNetworkOutput() const {
  return _output;
}

const gero::physics::BodyWeakRef
CarAgent::getBody() const {
  return _physicBody;
}

const gero::physics::VehicleWeakRef
CarAgent::getVehicle() const {
  return _physicVehicle;
}

float
CarAgent::getLife() const {
  return float(_health) / constants::healthMaxValue;
}

uint32_t
CarAgent::getTotalUpdates() const {
  return _totalUpdateNumber;
}
