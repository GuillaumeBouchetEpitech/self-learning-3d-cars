
#include "CarDataFrameHandler.hpp"

#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/math/clamp.hpp"

namespace {
constexpr std::size_t k_totalFrames = 10;

} // namespace

void
CarDataFrameHandler::initialize(uint32_t totalGenomes, float logicFrameDuration) {
  _totalGenomes = totalGenomes;
  _logicFrameDuration = logicFrameDuration;

  _allCarsData.resize(totalGenomes);
  for (std::size_t ii = 0; ii < k_totalFrames; ++ii) {
    AllCarsData newFrame;
    newFrame.resize(totalGenomes);
    _unusedFrames.push_back(std::move(newFrame));
  }
}

bool
CarDataFrameHandler::needNewFrame() const {
  return _usedFrames.size() < k_totalFrames && _unusedFrames.size() > 0;
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

  if (_usedFrames.size() <= 2) {
    if (_interpolationValue > _logicFrameDuration) {
      _interpolationValue = 0.0f;
    }
  }

  _usedFrames.push_back(std::move(newFrame));
}

void
CarDataFrameHandler::update(float deltaTime) {

  if (_usedFrames.size() <= 2) {
    return;
  }

  _interpolationValue += deltaTime;
  if (_interpolationValue > _logicFrameDuration) {

    // add latest frame
    _unusedFrames.push_back(std::move(_usedFrames.front()));
    // discard oldest frame
    _usedFrames.pop_front();

    _interpolationValue -= _logicFrameDuration;
    if (_interpolationValue > _logicFrameDuration * 0.5f) {
      _interpolationValue = 0.0f;
    }

    if (
      _usedFrames.size() >= 2 &&
      _onGenomeDieCallback
    ) {
      auto it = _usedFrames.begin();
      auto& frameA = *it;
      auto& frameB = *(++it);

      for (uint32_t ii = 0; ii < _totalGenomes; ++ii) {

        const auto& genomeA = frameA.at(ii);
        const auto& genomeB = frameB.at(ii);

        const bool genomeJustDied = genomeA.isAlive && !genomeB.isAlive;

        if (!genomeJustDied) {
          continue;
        }

        _onGenomeDieCallback(genomeA);
      }
    }

    if (_usedFrames.size() <= 2) {
      return;
    }
  }

  // if (_usedFrames.size() <= 2) {
  //   return;
  // }

  const float rawCoef = _interpolationValue / _logicFrameDuration;
  const float coef = gero::math::clamp(rawCoef, 0.0f, 1.0f);

  auto it = _usedFrames.begin();
  auto& frameA = *it;
  auto& frameB = *(++it);

  {
    for (uint32_t ii = 0; ii < _totalGenomes; ++ii) {

      // replace currently displayed frame
      _allCarsData.at(ii) = frameA.at(ii);

      if (!_allCarsData.at(ii).isAlive) {
        continue;
      }

      // interpolate the displayed frame
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

void
CarDataFrameHandler::setOnGenomeDieCallback(const GenomeDieCallback& callback) {
  _onGenomeDieCallback = callback;
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
