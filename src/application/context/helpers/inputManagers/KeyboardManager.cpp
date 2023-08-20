
#include "KeyboardManager.hpp"

#include "geronimo/system/ErrorHandler.hpp"

KeyboardManager* KeyboardManager::_instance = nullptr;

void
KeyboardManager::create() {
  if (_instance)
    D_THROW(std::runtime_error, "already created");

  _instance = new KeyboardManager();
}

void
KeyboardManager::destroy() {
  if (!_instance)
    D_THROW(std::runtime_error, "no allocated");

  delete _instance, _instance = nullptr;
}

KeyboardManager&
KeyboardManager::get() {
  if (!_instance)
    D_THROW(std::runtime_error, "no allocated");

  return *_instance;
}

void
KeyboardManager::updateAsPressed(int inKey) {
  _pressedKeys.insert(inKey);
}

void
KeyboardManager::updateAsReleased(int inKey) {
  _pressedKeys.erase(inKey);
}

bool
KeyboardManager::isPressed(int inKey) {
  return _pressedKeys.count(inKey) > 0;
}

void
KeyboardManager::reset() {
  _pressedKeys.clear();
}
