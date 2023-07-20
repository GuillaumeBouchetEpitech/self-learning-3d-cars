
#include "AbstractFlockingManager.hpp"

#include "internals/FlockingManager.hpp"

std::unique_ptr<AbstractFlockingManager> AbstractFlockingManager::create()
{
  return std::make_unique<FlockingManager>();
}

