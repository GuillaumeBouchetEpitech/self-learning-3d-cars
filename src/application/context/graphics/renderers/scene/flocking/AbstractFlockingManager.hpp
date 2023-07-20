
#pragma once

#include <memory>

class AbstractFlockingManager {
public:
  virtual ~AbstractFlockingManager() = default;

public:
  virtual void update(float elapsedTime) = 0;
  virtual void render() = 0;

public:
  static std::unique_ptr<AbstractFlockingManager> create();

};
