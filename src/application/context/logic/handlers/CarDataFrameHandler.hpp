
#pragma once

#include "application/context/logic/simulation/AbstractSimulation.hpp"
#include "application/context/logic/simulation/logic/CarData.hpp"

#include "geronimo/system/NonCopyable.hpp"

#include <functional>
#include <list>

class CarDataFrameHandler : public gero::NonCopyable {
public:
  using GenomeDieCallback = std::function<void(const CarData&)>;

public:
  CarDataFrameHandler() = default;
  ~CarDataFrameHandler() = default;

public:
  void initialize(uint32_t totalGenomes, float logicFrameDuration);
  bool needNewFrame() const;
  void pushNewFrame(const AbstractSimulation& simulation);
  void update(float deltaTime);
  void discardAll();

public:
  void setOnGenomeDieCallback(const GenomeDieCallback& callback);

public:
  uint32_t getTotalStoredFrames() const;
  const AllCarsData& getAllCarsData() const;
  float getLogicFrameDuration() const;

private:
  float _logicFrameDuration = 0.0f;
  uint32_t _totalGenomes = 0;

  std::list<AllCarsData> _unusedFrames;
  std::list<AllCarsData> _usedFrames;

  float _interpolationValue = 0.0f;

  AllCarsData _allCarsData;

  GenomeDieCallback _onGenomeDieCallback;
};
