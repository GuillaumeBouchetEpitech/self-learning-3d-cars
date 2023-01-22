
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace gero {
namespace math {

bool isInvalidInteger(int32_t inVal);
bool isInvalidInteger(uint32_t inVal);
bool isInvalidFloat(float inVal);

//

bool isInvalidVec2i(const glm::ivec2& inVec2);
bool isInvalidVec2i(const glm::uvec2& inVec2);
bool isInvalidVec2f(const glm::vec2& inVec2);

//

bool isInvalidVec3i(const glm::ivec3& inVec3);
bool isInvalidVec3i(const glm::uvec3& inVec3);
bool isInvalidVec3f(const glm::vec3& inVec3);

} // namespace math
} // namespace gero
