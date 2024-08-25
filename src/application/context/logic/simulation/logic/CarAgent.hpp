
#pragma once

#include "CarData.hpp"

#include "geronimo/physics/AbstractPhysicWorld.hpp"
#include "geronimo/physics/vehicle/AbstractPhysicVehicle.hpp"

#include "basic-genetic-algorithm/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <vector>

class CarAgent {
public: // external structures
  struct Sensor {
    glm::vec3 near;
    glm::vec3 far;
    float value = 0.0f;
  };
  using Sensors = std::array<Sensor, 15>;

  struct NeuralNetworkOutput {
    float steer = 0.0f;
    float speed = 0.0f;
  };

public: // ctor/dtor
  CarAgent() = default;

public: // method(s)
  void update(float elapsedTime, NeuralNetwork& nn);
  void reset(
    gero::physics::AbstractPhysicWorld* inPhysicWorld, const glm::vec3& position, const glm::vec4& quaternion,
    const glm::vec3& linearVelocity);

  void getAsCarData(const NeuralNetwork& inNeuralNet, CarData& inCarData) const;

private: // method(s)
  void _createVehicle();
  void _updateSensors();
  void _collideEyeSensors();
  bool _collideGroundSensor();

public: // setter(s)/getter(s)
  bool isOwnedByPhysicWorld(const gero::physics::AbstractPhysicWorld* inPhysicWorld) const;
  const Sensors& getEyeSensors() const;
  const Sensor& getGroundSensor() const;
  float getFitness() const;
  bool isAlive() const;
  bool isDying() const;
  int32_t getGroundIndex() const;
  const NeuralNetworkOutput& getNeuralNetworkOutput() const;
  const gero::physics::BodyWeakRef getBody() const;
  const gero::physics::VehicleWeakRef getVehicle() const;
  float getLife() const;
  uint32_t getTotalUpdates() const;

private: // attributes
  gero::physics::AbstractPhysicWorld* _physicWorld = nullptr;
  gero::physics::VehicleWeakRef _physicVehicle;
  gero::physics::BodyWeakRef _physicBody;

  float _fitness;
  float _health;
  uint32_t _totalUpdateNumber;

  Sensors _eyeSensors;
  Sensor _groundSensor;

  int32_t _groundIndex;
  bool _isDying;

  NeuralNetworkOutput _output;
};

using CarAgents = std::vector<CarAgent>;
