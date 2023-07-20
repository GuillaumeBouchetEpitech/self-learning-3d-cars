
#include "../AbstractFlockingManager.hpp"

#include "Boid.hpp"

#include "geronimo/system/trees/Spatial3dIndexer.hpp"


class FlockingManager : public AbstractFlockingManager {
public:
  FlockingManager();
  ~FlockingManager() = default;

public:
  void update(float elapsedTime) override;
  void render() override;

private:
  std::vector<Boid> _allBoids;

  float _timeUntilTrailUpdate = 0.0f;

  gero::trees::Spatial3dIndexer<Boid*> _spatialIndexer;

};
