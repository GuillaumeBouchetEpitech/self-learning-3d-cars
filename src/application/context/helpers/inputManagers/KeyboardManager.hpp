
#pragma once

#include <unordered_set>

class KeyboardManager {
private:
  KeyboardManager() = default;
  ~KeyboardManager() = default;

private:
  static KeyboardManager* _instance;

public:
  static void create();
  static void destroy();
  static KeyboardManager& get();

public:
  void updateAsPressed(int inKey);
  void updateAsReleased(int inKey);
  bool isPressed(int inKey);
  void reset();

  template <typename... Args> bool isPressed(Args... args) {
    std::array<int, sizeof...(args)> allValues{{args...}};

    for (int val : allValues)
      if (_pressedKeys.count(val) > 0)
        return true;
    return false;
  }

private:
  std::unordered_set<int> _pressedKeys;
};
