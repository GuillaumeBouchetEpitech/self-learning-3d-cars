
#include "CarDataFrameHandler.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/math/clamp.hpp"

void
CarDataFrameHandler::initialize(uint32_t totalGenomes, float logicFrameDuration) {
  _totalGenomes = totalGenomes;
  _logicFrameDuration = logicFrameDuration;

  _allCarsData.resize(totalGenomes);
  for (int ii = 0; ii < 10; ++ii) {
    AllCarsData newFrame;
    newFrame.resize(totalGenomes);
    _unusedFrames.push_back(std::move(newFrame));
  }
}

bool
CarDataFrameHandler::needNewFrame() const {
  return _usedFrames.size() <= 9 && !_unusedFrames.empty();
}

void
CarDataFrameHandler::pushNewFrame(const AbstractSimulation& simulation) {
  if (_unusedFrames.empty()) {
    D_THROW(std::runtime_error, "not more unused frames");
  }

  if (_unusedFrames.back().size() != simulation.getTotalCars()) {
    D_THROW(
      std::runtime_error, "new frame match not matching"
                            << ", expected: " << _unusedFrames.back().size()
                            << ", input: " << simulation.getTotalCars());
  }

  AllCarsData newFrame = std::move(_unusedFrames.back());
  _unusedFrames.pop_back();

  for (uint32_t ii = 0; ii < _totalGenomes; ++ii)
    newFrame.at(ii) = simulation.getCarResult(ii);

  _usedFrames.push_back(std::move(newFrame));
}

void
CarDataFrameHandler::update(float deltaTime) {
  if (_usedFrames.size() < 3)
    return;

  _interpolationValue += deltaTime;
  if (_interpolationValue > _logicFrameDuration) {
    // discard oldest frame
    _unusedFrames.push_back(std::move(_usedFrames.front()));
    _usedFrames.pop_front();

    _interpolationValue -= _logicFrameDuration;
  }

  // D_MYLOG("_interpolationValue " << _interpolationValue);

  const float coef = gero::math::clamp(_interpolationValue / _logicFrameDuration, 0.0f, 1.0f);

  // D_MYLOG("coef " << coef);

  auto it = _usedFrames.begin();
  auto& frameA = *it;
  auto& frameB = *(++it);

  {
    for (uint32_t ii = 0; ii < _totalGenomes; ++ii) {
      // const auto& carData = simulation.getCarResult(ii);

      // _allCarsData.at(ii) = carData;
      _allCarsData.at(ii) = frameA.at(ii);

      if (!_allCarsData.at(ii).isAlive)
        continue;

      _allCarsData.at(ii).lerp(frameA.at(ii), frameB.at(ii), coef);
    }
  }
}

void
CarDataFrameHandler::discardAll() {
  // discard all used frames
  for (auto& frame : _usedFrames)
    _unusedFrames.push_back(std::move(frame));
  _usedFrames.clear();

  for (unsigned int ii = 0; ii < _allCarsData.size(); ++ii)
    _allCarsData.at(ii).isAlive = false;
}

uint32_t
CarDataFrameHandler::getTotalStoredFrames() const {
  return uint32_t(_usedFrames.size());
}

const AllCarsData&
CarDataFrameHandler::getAllCarsData() const {
  return _allCarsData;
}

float
CarDataFrameHandler::getLogicFrameDuration() const {
  return _logicFrameDuration;
}
