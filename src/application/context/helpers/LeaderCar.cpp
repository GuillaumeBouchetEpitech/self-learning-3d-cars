
#include "LeaderCar.hpp"

#include "application/context/Context.hpp"
#include "application/states/StateManager.hpp"

void
LeaderCar::update(float elapsedTime) {

  const StateManager::States currentState = StateManager::get()->getState();
  const bool validSate = (currentState == StateManager::States::Running);
  if (!validSate) {
    reset();
    return;
  }

  auto& allCarsData = Context::get().logic.carDataFrameHandler.getAllCarsData();
  if (_countdownUntilNewLeader > 0.0f)
    _countdownUntilNewLeader -= elapsedTime;

  _totalTimeAsLeader += elapsedTime;

  if (
    // no leader yet
    _carIndex == -1 ||
    // the current leader is dead
    allCarsData.at(_carIndex).isAlive == false ||
    // the current leader is not on the ground
    allCarsData.at(_carIndex).groundSensor.value > 0.5f ||
    // time to check for a potentially better leader
    _countdownUntilNewLeader <= 0.0f) {
    // reset the timeout
    _countdownUntilNewLeader = 1.0f; // <= one second

    // refresh the currently best car

    unsigned int totalCars = allCarsData.size();

    float bestFitness = 0.0f;
    int oldLeaderCarIndex = _carIndex;
    _carIndex = -1;
    for (unsigned int carIndex = 0; carIndex < totalCars; ++carIndex) {
      const auto& carData = allCarsData.at(carIndex);

      if (!carData.isAlive)
        continue;

      if (_carIndex != -1 && !(carData.fitness > bestFitness + 2.0f)) {
        continue;
      }

      bestFitness = carData.groundIndex;
      _carIndex = carIndex;
    }

    if (_carIndex >= 0 && _carIndex != oldLeaderCarIndex)
      _totalTimeAsLeader = 0.0f; // new leader
  }

  // do we have a car to focus the gero::graphics::Camera on?
  if (_carIndex >= 0) {
    const auto& carResult = allCarsData.at(_carIndex);

    // this part elevate where the gero::graphics::Camera look along the up axis
    // of the car
    // => without it the gero::graphics::Camera look at the ground
    // => mostly useful for a shoulder gero::graphics::Camera0
    const auto& chassis = carResult.liveTransforms.chassis;
    _carPosition = chassis.position + glm::mat3_cast(chassis.orientation) *
                                        glm::vec3(0.0f, 0.0f, 2.0f);
  }
}

void
LeaderCar::reset() {
  _carIndex = -1;
  _countdownUntilNewLeader = 0.0f;
  _totalTimeAsLeader = 0.0f;
  _carPosition = {0, 0, 0};
}

bool
LeaderCar::hasLeader() const {
  return _carIndex >= 0;
}

int
LeaderCar::leaderIndex() const {
  return _carIndex;
}

std::optional<CarData>
LeaderCar::leaderData() const {
  if (_carIndex < 0)
    return {};

  return Context::get().logic.carDataFrameHandler.getAllCarsData().at(
    _carIndex);
}

std::optional<glm::vec3>
LeaderCar::leaderPosition() const {
  if (_carIndex < 0)
    return {};

  return _carPosition;
}

float
LeaderCar::totalTimeAsLeader() const {
  return _totalTimeAsLeader;
}
