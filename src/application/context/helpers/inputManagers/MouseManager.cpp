
#include "MouseManager.hpp"

#include "geronimo/system/ErrorHandler.hpp"

MouseManager* MouseManager::_instance = nullptr;

void
MouseManager::create() {
  if (_instance)
    D_THROW(std::runtime_error, "already created");

  _instance = new MouseManager();
}

void
MouseManager::destroy() {
  if (!_instance)
    D_THROW(std::runtime_error, "no allocated");

  delete _instance, _instance = nullptr;
}

MouseManager&
MouseManager::get() {
  if (!_instance)
    D_THROW(std::runtime_error, "no allocated");

  return *_instance;
}

//

void
MouseManager::updateAsPressed(int inKey) {
  _pressedKeys.insert(inKey);
}

void
MouseManager::updateAsReleased(int inKey) {
  _pressedKeys.erase(inKey);
}

void
MouseManager::updateDelta(float x, float y) {
  _delta.x += x;
  _delta.y += y;
}

//

bool
MouseManager::isPressed(int inKey) const {
  return _pressedKeys.count(inKey) > 0;
}

const glm::vec2&
MouseManager::getDelta() const {
  return _delta;
}

void
MouseManager::resetDelta() {
  _delta = {0, 0};
}

void
MouseManager::reset() {
  _pressedKeys.clear();
  resetDelta();
}

//

bool
MouseManager::isLocked() const {
  return _isLocked;
}

bool
MouseManager::setLock(bool isLocked) {
  if (_isLocked == isLocked)
    return false;
  _isLocked = isLocked;
  return true;
}
