
#include "PhysicBodyManager.hpp"

#include "geronimo/helpers/internals/BulletPhysics.hpp"
#include "geronimo/physic/PhysicWorld.hpp"
#include "geronimo/system/TraceLogger.hpp"

namespace gero {
namespace physic {

PhysicBodyManager::PhysicBodyManager(PhysicWorld& physicWorld)
  : _physicWorld(physicWorld) {
  _bodies.pre_allocate(1024);
}

PhysicBodyManager::~PhysicBodyManager() { clear(); }

void PhysicBodyManager::clear() {
  _bodies.for_each([this](AbstractPhysicBody& body) { removeBody(body); });
  _bodies.clear();
}

PhysicBodyManager::BodyWeakRef
PhysicBodyManager::createBody(const PhysicBodyDef& def) {
  return _bodies.acquire(def);
}

PhysicBodyManager::BodyWeakRef
PhysicBodyManager::createAndAddBody(const PhysicBodyDef& def) {
  auto ref = createBody(def);
  addBody(ref, def.group, def.mask);
  return ref;
}

void PhysicBodyManager::destroyBody(PhysicBodyManager::BodyWeakRef ref) {
  if (!ref)
    return;

  removeBody(ref);
  _bodies.release(ref);
}

void PhysicBodyManager::addBody(PhysicBodyManager::BodyWeakRef ref, short group,
                                short mask) {
  if (!ref)
    return;
  PhysicBody* implementation = reinterpret_cast<PhysicBody*>(ref.get());
  if (implementation->_isAdded)
    return;
  _physicWorld._bullet.dynamicsWorld->addRigidBody(implementation->_bullet.body,
                                                   group, mask);
  implementation->_isAdded = true;
}

void PhysicBodyManager::removeBody(AbstractPhysicBody& body) {
  PhysicBody& implementation = reinterpret_cast<PhysicBody&>(body);
  if (!implementation._isAdded)
    return;
  _physicWorld._bullet.dynamicsWorld->removeRigidBody(
    implementation._bullet.body);
  implementation._isAdded = false;
}

void PhysicBodyManager::removeBody(PhysicBodyManager::BodyWeakRef ref) {
  if (!ref)
    return;
  removeBody(*ref.get());
}

PhysicBodyManager::BodyWeakRef PhysicBodyManager::getBody(unsigned int index) {
  return _bodies.get(index);
}

const PhysicBodyManager::BodyWeakRef
PhysicBodyManager::getBody(unsigned int index) const {
  return _bodies.get(index);
}

// PhysicBodyManager::BodyWeakRef PhysicBodyManager::getBody(const
// AbstractPhysicBody& inBody)
// {
//   // return _bodies.get(inBody);
//   return _bodies.find_if([&inBody](const AbstractPhysicBody& body){ return
//   &body == &inBody; });
// }

// const PhysicBodyManager::BodyWeakRef PhysicBodyManager::getBody(const
// AbstractPhysicBody& inBody) const
// {
//   // return _bodies.get(inBody);
//   return _bodies.find_if([&inBody](const AbstractPhysicBody& body){ return
//   &body == &inBody; });
// }

std::size_t PhysicBodyManager::size() const { return _bodies.size(); }

bool PhysicBodyManager::empty() const { return _bodies.is_empty(); }

void PhysicBodyManager::dumpData() {
  D_MYERR("########");
  _bodies.for_each([](AbstractPhysicBody& body) { body.dumpData(); });
}

} // namespace physic
} // namespace gero
