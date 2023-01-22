
#include "transformConversion.hpp"

#include "geronimo/helpers/internals/BulletPhysics.hpp"

namespace gero {
namespace physic {

void setBulletFromGlm(const glm::mat4& inMat4x4, btScalar* outMat4x4) {
  // _bullet.body->getWorldTransform().setFromOpenGLMatrix(glm::value_ptr(inMat4x4));

  // btScalar tmpMat[16];
  for (int ii = 0; ii < 4; ++ii) {
    outMat4x4[ii * 4 + 0] = btScalar(inMat4x4[ii].x);
    outMat4x4[ii * 4 + 1] = btScalar(inMat4x4[ii].y);
    outMat4x4[ii * 4 + 2] = btScalar(inMat4x4[ii].z);
    outMat4x4[ii * 4 + 3] = btScalar(inMat4x4[ii].w);
  }
  // _bullet.body->getWorldTransform().setFromOpenGLMatrix(tmpMat);
}

void getGlmFromBullet(const btScalar* inMat4, glm::mat4& outMat4x4) {
  for (int ii = 0; ii < 4; ++ii) {
    outMat4x4[ii].x = float(inMat4[ii * 4 + 0]);
    outMat4x4[ii].y = float(inMat4[ii * 4 + 1]);
    outMat4x4[ii].z = float(inMat4[ii * 4 + 2]);
    outMat4x4[ii].w = float(inMat4[ii * 4 + 3]);
  }
}

} // namespace physic
} // namespace gero
