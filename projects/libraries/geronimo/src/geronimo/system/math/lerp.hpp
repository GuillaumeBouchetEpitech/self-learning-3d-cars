
#pragma once

namespace gero {
namespace math {

template <typename T> T lerp(T valueA, T valueB, float coef) {
  return valueA + (valueB - valueA) * coef;
}

} // namespace math
} // namespace gero
