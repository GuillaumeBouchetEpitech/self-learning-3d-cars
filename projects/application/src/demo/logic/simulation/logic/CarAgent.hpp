
#pragma once

#include "geronimo/physic/PhysicWorld.hpp"
#include "geronimo/physic/vehicle/PhysicVehicle.hpp"

#include "machine-learning/NeuralNetwork.hpp"

#include "geronimo/helpers/GLMath.hpp"

#include <array>
#include <vector>

class CarAgent {
public: // external structures
  struct Sensor {
    glm::vec3 near, far;
    float value = 0.0f;
  };
  using Sensors = std::array<Sensor, 15>;

  struct NeuralNetworkOutput {
    float steer = 0.0f;
    float speed = 0.0f;
  };

private: // attributes
  gero::physic::PhysicWorld* _physicWorld = nullptr;
  gero::physic::PhysicVehicleManager::VehicleWeakRef _physicVehicle;
  gero::physic::PhysicBodyManager::BodyWeakRef _physicBody;

  float _fitness;
  bool _isAlive;
  float _health;
  unsigned int _totalUpdateNumber;

  // TODO : use enumeration
  Sensors _eyeSensors;
  Sensor _groundSensor;

  int _groundIndex;

  NeuralNetworkOutput _output;

public: // ctor/dtor
  CarAgent() = default;

public: // methods
  void update(float elapsedTime, const std::shared_ptr<NeuralNetwork> nn);
  void reset(
    gero::physic::PhysicWorld* inPhysicWorld, const glm::vec3& position,
    const glm::vec4& quaternion);

private: // methods
  void _createVehicle();
  void _updateSensors();
  void _collideEyeSensors();
  bool _collideGroundSensor();

public: // setter/getter
  const Sensors& getEyeSensors() const;
  const Sensor& getGroundSensor() const;
  float getFitness() const;
  bool isAlive() const;
  int getGroundIndex() const;
  const NeuralNetworkOutput& getNeuralNetworkOutput() const;
  const gero::physic::PhysicBodyManager::BodyWeakRef getBody() const;
  const gero::physic::PhysicVehicleManager::VehicleWeakRef getVehicle() const;
  float getLife() const;
  unsigned int getTotalUpdates() const;
};

using CarAgents = std::vector<CarAgent>;
