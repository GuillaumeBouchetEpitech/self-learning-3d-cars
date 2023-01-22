
#include "validation-helpers.hpp"

#include <cmath>

namespace gero {
namespace math {

bool isInvalidInteger(int32_t inVal) {
  return std::isnan(inVal) || std::isinf(inVal);
}
bool isInvalidInteger(uint32_t inVal) {
  return std::isnan(inVal) || std::isinf(inVal);
}
bool isInvalidFloat(float inVal) {
  return std::isnan(inVal) || std::isinf(inVal);
}

//

bool isInvalidVec2i(const glm::ivec2& inVec2) {
  return isInvalidInteger(inVec2.x) || isInvalidInteger(inVec2.y);
}
bool isInvalidVec2i(const glm::uvec2& inVec2) {
  return isInvalidInteger(inVec2.x) || isInvalidInteger(inVec2.y);
}
bool isInvalidVec2f(const glm::vec2& inVec2) {
  return isInvalidFloat(inVec2.x) || isInvalidFloat(inVec2.y);
}

//

bool isInvalidVec3i(const glm::ivec3& inVec3) {
  return isInvalidInteger(inVec3.x) || isInvalidInteger(inVec3.y) ||
         isInvalidInteger(inVec3.z);
}
bool isInvalidVec3i(const glm::uvec3& inVec3) {
  return isInvalidInteger(inVec3.x) || isInvalidInteger(inVec3.y) ||
         isInvalidInteger(inVec3.z);
}
bool isInvalidVec3f(const glm::vec3& inVec3) {
  return isInvalidFloat(inVec3.x) || isInvalidFloat(inVec3.y) ||
         isInvalidFloat(inVec3.z);
}

} // namespace math
} // namespace gero
