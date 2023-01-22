
#include "QueryShape.hpp"

#include "geronimo/helpers/internals/BulletPhysics.hpp"
#include "geronimo/physic/PhysicWorld.hpp"
#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

namespace gero {
namespace physic {

namespace {

/// ContactResultCallback is used to report contact points
struct MyContactResultCallback
  : public btCollisionWorld::ContactResultCallback {
  int m_bodyUniqueIdA;
  int m_bodyUniqueIdB;
  int m_linkIndexA;
  int m_linkIndexB;
  btScalar m_deltaTime;

  PhysicWorld& _physicWorld;

  btPairCachingGhostObject& _volume;
  QueryShape::QueryShapeParams::ResultRaw& _result;
  void* _toIgnore;

  MyContactResultCallback(PhysicWorld& physicWorld,
                          btPairCachingGhostObject& volume,
                          QueryShape::QueryShapeParams::ResultRaw& result,
                          int collisionGroup, int collisionMask, void* toIgnore)
    : _physicWorld(physicWorld), _volume(volume), _result(result),
      _toIgnore(toIgnore) {
    m_collisionFilterGroup = collisionGroup;
    m_collisionFilterMask = collisionMask;
  }

  virtual ~MyContactResultCallback() {}

  virtual bool needsCollision(btBroadphaseProxy* proxy0) const {
    return ((proxy0->m_collisionFilterGroup & m_collisionFilterMask) != 0 &&
            (m_collisionFilterGroup & proxy0->m_collisionFilterMask));
  }

  virtual btScalar addSingleResult(btManifoldPoint& cp,
                                   const btCollisionObjectWrapper* colObj0Wrap,
                                   int partId0, int index0,
                                   const btCollisionObjectWrapper* colObj1Wrap,
                                   int partId1, int index1) {
    static_cast<void>(cp);      // unused
    static_cast<void>(partId0); // unused
    static_cast<void>(index0);  // unused
    static_cast<void>(partId1); // unused
    static_cast<void>(index1);  // unused

    if (_result.allBodiesTotal >= _result.allBodiesMaxSize)
      return 1;

    const btCollisionObjectWrapper* pObject =
      (colObj0Wrap->m_collisionObject != &_volume ? colObj0Wrap : colObj1Wrap);
    // PhysicBody* pBody =
    //   static_cast<PhysicBody*>(pObject->m_collisionObject->getUserPointer());

    const btRigidBody* pRigidBody =
      btRigidBody::upcast(pObject->m_collisionObject);
    if (pRigidBody == nullptr || !pRigidBody->hasContactResponse() ||
        pRigidBody->getUserPointer() == nullptr ||
        (_toIgnore != nullptr && pRigidBody->getUserPointer() == _toIgnore)) {
      return 1;
    }

    AbstractPhysicBody* pPhysicBody =
      static_cast<AbstractPhysicBody*>(pRigidBody->getUserPointer());

    // check duplicates
    for (std::size_t ii = 0; ii < _result.allBodiesTotal; ++ii)
      if (_result.allRawBodiesData[ii] == pPhysicBody)
        // if (_result.allRawBodiesRefs[ii].get() == pPhysicBody)
        return 1;

    _result.allRawBodiesData[_result.allBodiesTotal] = pPhysicBody;
    // _result.allRawBodiesRefs[_result.allBodiesTotal] =
    // _physicWorld.getPhysicBodyManager().getBody(*pPhysicBody);
    _result.allBodiesTotal += 1;
    return 1;
  }
};

} // namespace

QueryShape::QueryShape(PhysicWorld& physicWorld) : _physicWorld(physicWorld) {}

bool QueryShape::_queryShape(QueryShapeParams& inParams,
                             QueryShapeParams::ResultRaw& outResultArray) {
  outResultArray.allBodiesTotal = 0;

  PhysicShape* pShape = PhysicShape::create(inParams.shape, false);

  // if (inParams.radius <= 0.0f)
  //   return false;

  // btSphereShape sphereShape(inParams.radius);

  btPairCachingGhostObject volume = btPairCachingGhostObject();
  // volume.setCollisionShape(&sphereShape);
  volume.setCollisionShape(pShape->getRawShape());
  volume.setCollisionFlags(volume.getCollisionFlags() |
                           btCollisionObject::CF_NO_CONTACT_RESPONSE);

  btTransform tr;
  tr.setIdentity();
  tr.setOrigin(
    btVector3(inParams.position.x, inParams.position.y, inParams.position.z));
  volume.setWorldTransform(tr);

  MyContactResultCallback cr(_physicWorld, volume, outResultArray,
                             inParams.collisionGroup, inParams.collisionMask,
                             inParams.toIgnore);

  _physicWorld._bullet.dynamicsWorld->contactTest(&volume, cr);

  delete pShape;

  outResultArray.hasHit = outResultArray.allBodiesTotal > 0;
  return outResultArray.hasHit;
}

} // namespace physic
} // namespace gero
