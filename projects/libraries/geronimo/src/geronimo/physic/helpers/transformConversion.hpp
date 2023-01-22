
#pragma once

#include "geronimo/helpers/GLMath.hpp"

namespace gero {
namespace physic {

#ifdef BT_USE_DOUBLE_PRECISION
void setBulletFromGlm(const glm::mat4& inMat4x4, double* outMat4x4);
void getGlmFromBullet(const double* inMat4, glm::mat4& outMat4x4);
#else
void setBulletFromGlm(const glm::mat4& mat4x4, float* outMat4x4);
void getGlmFromBullet(const float* inMat4, glm::mat4& outMat4x4);
#endif

} // namespace physic
} // namespace gero
