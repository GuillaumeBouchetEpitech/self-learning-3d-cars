
#pragma once

#include "application/context/simulation/logic/CarData.hpp"
#include "application/context/simulation/AbstractSimulation.hpp"

#include <list>

class CarDataFrameHandler
{
public:
  CarDataFrameHandler() = default;
  ~CarDataFrameHandler() = default;

public:
  void initalize(uint32_t totalGenomes);
  bool needNewFrame() const;
  void pushNewFrame(const AbstractSimulation& simulation);
  void update(float deltaTime);
  void discardAll();

public:
  const AllCarsData& getAllCarsData() const;

private:
  uint32_t _totalGenomes = 0;

  std::list<AllCarsData> _unusedFrames;
  std::list<AllCarsData> _usedFrames;

  float _interpolationValue = 0.0f;

  AllCarsData _allCarsData;

};

