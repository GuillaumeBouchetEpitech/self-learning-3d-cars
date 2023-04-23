
#include "CarDataFrameHandler.hpp"

#include "geronimo/system/ErrorHandler.hpp"

void CarDataFrameHandler::initalize(uint32_t totalGenomes)
{
  _totalGenomes = totalGenomes;

  _allCarsData.resize(totalGenomes);
  for (int ii = 0; ii < 4; ++ii)
  {
    _unusedFrames.push_back(AllCarsData());
    _unusedFrames.back().resize(totalGenomes);
  }

}

bool CarDataFrameHandler::needNewFrame() const
{
  return _usedFrames.size() <= 3 && !_unusedFrames.empty();
}

void CarDataFrameHandler::pushNewFrame(const AbstractSimulation& simulation)
{
  if (_unusedFrames.empty())
    D_THROW(std::runtime_error, "not more unused frames");

  if (_unusedFrames.back().size() != simulation.getTotalCars())
    D_THROW(std::runtime_error, "new frame match not matching"
                                << ", expected: " << _unusedFrames.back().size()
                                << ", input: " << simulation.getTotalCars());

  AllCarsData newFrame = std::move(_unusedFrames.back());
  _unusedFrames.pop_back();


  for (uint32_t ii = 0; ii < _totalGenomes; ++ii)
    newFrame.at(ii) = simulation.getCarResult(ii);


  _usedFrames.push_back(std::move(newFrame));

}

void CarDataFrameHandler::update(float deltaTime)
{
  if (_usedFrames.size() < 3)
    return;


  constexpr float logicFrameDuration = 1.0f / 50.0f; // TODO: hardcoded

  _interpolationValue += std::min(deltaTime, logicFrameDuration);
  if (_interpolationValue > logicFrameDuration)
  {
    // discard oldest frame
    _unusedFrames.push_back(std::move(_usedFrames.front()));
    _usedFrames.pop_front();

    _interpolationValue -= logicFrameDuration;
  }

  // D_MYLOG("_interpolationValue " << _interpolationValue);

  const float coef = _interpolationValue / logicFrameDuration;

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

void CarDataFrameHandler::discardAll()
{
  // discard all used frames
  while (!_usedFrames.empty())
  {
    _unusedFrames.push_back(std::move(_usedFrames.front()));
    _usedFrames.pop_front();
  }

  for (unsigned int ii = 0; ii < _allCarsData.size(); ++ii)
    _allCarsData.at(ii).isAlive = false;
}

const AllCarsData& CarDataFrameHandler::getAllCarsData() const
{
  return _allCarsData;
}


