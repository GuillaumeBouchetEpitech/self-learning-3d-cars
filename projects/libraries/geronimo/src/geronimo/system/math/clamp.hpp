
#pragma once

#include <algorithm>

namespace gero {
namespace math {

template <typename T> T clamp(T value, T minVal, T maxVal) {
  return std::min(std::max(value, minVal), maxVal);
}

} // namespace math
} // namespace gero