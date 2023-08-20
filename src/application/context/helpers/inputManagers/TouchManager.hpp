
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/system/NonCopyable.hpp"

#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>

#include <optional>
#include <string>
#include <unordered_map>

struct TouchData {
  int32_t fingerId = -1;
  glm::vec2 position = {0.0f, 0.0f};
  glm::vec2 delta = {0.0f, 0.0f};

  // TouchData(int32_t inFingerId = -1, const glm::vec2& inPos = {0,0});
  TouchData() = default;
  ~TouchData() = default;
  // TouchData(const TouchData& other);
  // TouchData(TouchData&& other);
  // TouchData& operator=(const TouchData& other);
  // TouchData& operator=(TouchData&& other);
};

class TouchManager : public gero::NonCopyable {
public:
  using MaybeTouchDataRef = std::optional<std::reference_wrapper<TouchData>>;

private:
  TouchManager() = default;
  ~TouchManager() = default;

private:
  static TouchManager* _instance;

public:
  static void create();
  static void destroy();
  static TouchManager& get();

public:
  void updateAsTouchedDown(int32_t inFingerId, const glm::vec2& inPos);
  void updateAsTouchedUp(int32_t inFingerId);
  void updateAsTouchedMotion(int32_t inFingerId, const glm::vec2& inPos, const glm::vec2& inDelta);

  // public:
  //   bool isPressed(int inKey) const;

  //   template <typename... Args> bool isPressed(Args... args) const {
  //     std::array<int, sizeof...(args)> allValues{{args...}};

  //     for (int val : allValues)
  //       if (isPressed(val))
  //         return true;
  //     return false;
  //   }

public:
  MaybeTouchDataRef getTouchData(int32_t inFingerId);

public:
  void reset();

private:
  std::unordered_map<int32_t, TouchData> _touchDataMap;
};
