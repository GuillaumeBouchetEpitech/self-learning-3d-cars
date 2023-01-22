
#include "Raycaster.hpp"

#include "geronimo/helpers/internals/BulletPhysics.hpp"
#include "geronimo/physic/PhysicWorld.hpp"
#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

namespace gero {
namespace physic {

Raycaster::Raycaster(PhysicWorld& physicWorld) : _physicWorld(physicWorld) {}

//
//
//
//
//
//
//
//
//
//

template <typename ResultCallback>
struct BaseCustomRayResultCallback : public ResultCallback {

  BaseCustomRayResultCallback(PhysicWorld& physicWorld, void* toIgnore,
                              Raycaster::RaycastParams::Type type,
                              Raycaster::RaycastParams::ResultRaw& result)
    : _physicWorld(physicWorld), _result(result), _toIgnore(toIgnore),
      _type(type) {}

  PhysicWorld& _physicWorld;

  Raycaster::RaycastParams::ResultRaw& _result;

  void* _toIgnore;
  Raycaster::RaycastParams::Type _type;

  bool _isValid(const btCollisionObject* pCollisionObject) const {
    const btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);
    return (
      (_type == Raycaster::RaycastParams::Type::closest ||
       _result.allImpactsTotal < _result.allImpactsMaxSize) &&
      pRigidBody && pRigidBody->hasContactResponse() &&
      pRigidBody->getUserPointer() != nullptr &&
      (_toIgnore == nullptr || pRigidBody->getUserPointer() != _toIgnore));
  }

  void _process(const btCollisionObject* pCollisionObject,
                const btVector3& hitNormalLocal, const btVector3& hitPointWorld,
                bool normalInWorldSpace) {

    const btRigidBody* pRigidBody = btRigidBody::upcast(pCollisionObject);

    btVector3 hitNormal;
    // btVector3 hitPos;
    if (normalInWorldSpace) {
      hitNormal = hitNormalLocal;
    } else {
      /// need to transform normal into worldspace
      hitNormal = pRigidBody->getWorldTransform().getBasis() * hitNormalLocal;
    }
    // hitPos = hitPointWorld;

    const glm::vec3 impactPoint =
      glm::vec3(hitPointWorld[0], hitPointWorld[1], hitPointWorld[2]);
    const glm::vec3 impactNormal =
      glm::vec3(hitNormal[0], hitNormal[1], hitNormal[2]);

    AbstractPhysicBody* pPhysicBody =
      static_cast<AbstractPhysicBody*>(pRigidBody->getUserPointer());

    // auto bodyRef = _physicWorld.getPhysicBodyManager().getBody(*physicBody);

    if (_type == Raycaster::RaycastParams::Type::closest) {
      _result.allRawImpactsData[0].impactPoint = impactPoint;
      _result.allRawImpactsData[0].impactNormal = impactNormal;
      _result.allRawImpactsData[0].body = pPhysicBody;
      // _result.allRawImpactsData[0].bodyRef = bodyRef;
      _result.allImpactsTotal = 1;
    } else {
      _result.allRawImpactsData[_result.allImpactsTotal].impactPoint =
        impactPoint;
      _result.allRawImpactsData[_result.allImpactsTotal].impactNormal =
        impactNormal;
      _result.allRawImpactsData[_result.allImpactsTotal].body = pPhysicBody;
      // _result.allRawImpactsData[_result.allImpactsTotal].bodyRef = bodyRef;
      _result.allImpactsTotal += 1;
    }
  }
};

//
//
//
//
//
//
//
//
//
//

struct CustomRayResultCallback
  : public BaseCustomRayResultCallback<btCollisionWorld::RayResultCallback> {

  CustomRayResultCallback(PhysicWorld& physicWorld,
                          const btVector3& rayFromWorld,
                          const btVector3& rayToWorld, void* toIgnore,
                          Raycaster::RaycastParams::Type type,
                          Raycaster::RaycastParams::ResultRaw& result)
    : BaseCustomRayResultCallback(physicWorld, toIgnore, type, result),
      _rayFromWorld(rayFromWorld), _rayToWorld(rayToWorld) {

    // //@BP Mod - allow backface filtering and unflipped normals
    // enum EFlags
    // {
    //   kF_None                 = 0,
    //   kF_FilterBackfaces      = 1 << 0,
    //   kF_KeepUnflippedNormal  = 1 << 1,   // Prevents returned face normal
    //   getting flipped when a ray hits a back-facing triangle
    //   ///SubSimplexConvexCastRaytest is the default, even if kF_None is set.
    //   kF_UseSubSimplexConvexCastRaytest = 1 << 2,   // Uses an approximate
    //   but faster ray versus convex intersection algorithm
    //   kF_UseGjkConvexCastRaytest = 1 << 3,
    //   kF_Terminator        = 0xFFFFFFFF
    // };

    // this->m_flags |= btTriangleRaycastCallback::EFlags::kF_FilterBackfaces;
    // this->m_flags |=
    // btTriangleRaycastCallback::EFlags::kF_KeepUnflippedNormal; this->m_flags
    // |= btTriangleRaycastCallback::EFlags::kF_UseSubSimplexConvexCastRaytest;
    // this->m_flags |=
    // btTriangleRaycastCallback::EFlags::kF_UseGjkConvexCastRaytest;
  }

  btVector3 _rayFromWorld;
  btVector3 _rayToWorld;

  virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult,
                                   bool normalInWorldSpace) override {

    m_collisionObject = rayResult.m_collisionObject;
    m_closestHitFraction = rayResult.m_hitFraction;

    if (_isValid(rayResult.m_collisionObject)) {

      btVector3 hitPos;
      hitPos.setInterpolate3(_rayFromWorld, _rayToWorld, m_closestHitFraction);

      _process(rayResult.m_collisionObject, rayResult.m_hitNormalLocal, hitPos,
               normalInWorldSpace);
    }

    if (_type == Raycaster::RaycastParams::Type::everything)
      return 1.0f;
    return m_closestHitFraction;
  }
};

void Raycaster::_normalRaycast(RaycastParams& params,
                               RaycastParams::ResultRaw& result) {
  // normal raycast

  btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
  btVector3 rayTo(params.to.x, params.to.y, params.to.z);

  CustomRayResultCallback rayCallback(_physicWorld, rayFrom, rayTo,
                                      params.toIgnore, params.type, result);
  rayCallback.m_collisionFilterGroup = params.collisionGroup;
  rayCallback.m_collisionFilterMask = params.collisionMask;

  _physicWorld._bullet.dynamicsWorld->rayTest(rayFrom, rayTo, rayCallback);

  result.hasHit = rayCallback.hasHit() && result.allImpactsTotal > 0;
}

//
//
//
//
//
//
//
//
//
//

struct CustomConvexResultCallback
  : public BaseCustomRayResultCallback<btCollisionWorld::ConvexResultCallback> {
  CustomConvexResultCallback(PhysicWorld& physicWorld, void* toIgnore,
                             Raycaster::RaycastParams::Type type,
                             Raycaster::RaycastParams::ResultRaw& result)
    : BaseCustomRayResultCallback(physicWorld, toIgnore, type, result) {}

  virtual btScalar
  addSingleResult(btCollisionWorld::LocalConvexResult& convexResult,
                  bool normalInWorldSpace) {

    // // caller already does the filter on the m_closestHitFraction
    // btAssert(convexResult.m_hitFraction <= m_closestHitFraction);

    m_closestHitFraction = convexResult.m_hitFraction;

    if (_isValid(convexResult.m_hitCollisionObject)) {
      _process(convexResult.m_hitCollisionObject, convexResult.m_hitNormalLocal,
               convexResult.m_hitPointLocal, normalInWorldSpace);
    }

    if (_type == Raycaster::RaycastParams::Type::everything)
      return 1.0f;
    return m_closestHitFraction;
  }
};

void Raycaster::_convexSweep(RaycastParams& params,
                             RaycastParams::ResultRaw& result) {

  btVector3 rayFrom(params.from.x, params.from.y, params.from.z);
  btVector3 rayTo(params.to.x, params.to.y, params.to.z);

  // replace raycast with convex sweep test

  btSphereShape sphereShape(params.radius);

  btTransform from;
  from.setIdentity();
  from.setOrigin(rayFrom);

  btTransform to;
  to.setIdentity();
  to.setOrigin(rayTo);

  CustomConvexResultCallback sweepCallback(_physicWorld, params.toIgnore,
                                           params.type, result);
  sweepCallback.m_collisionFilterGroup = params.collisionGroup;
  sweepCallback.m_collisionFilterMask = params.collisionMask;

  btScalar allowedCcdPenetration = 0.0f;

  _physicWorld._bullet.dynamicsWorld->convexSweepTest(
    &sphereShape, from, to, sweepCallback, allowedCcdPenetration);

  result.hasHit = sweepCallback.hasHit() && result.allImpactsTotal > 0;
}

//
//
//
//
//
//
//
//
//
//

bool Raycaster::_raycast(RaycastParams& params,
                         RaycastParams::ResultRaw& result) {
  result.allImpactsTotal = 0;

  if (params.radius <= 0.0f) {
    _normalRaycast(params, result);
  } else {
    _convexSweep(params, result);
  }

  return result.allImpactsTotal > 0;
}

} // namespace physic
} // namespace gero
