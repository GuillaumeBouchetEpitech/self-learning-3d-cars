
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physic/body/PhysicBodyManager.hpp"

#include <array>

namespace gero {
namespace physic {

class PhysicWorld;

class Raycaster {
  friend PhysicWorld;

private:
  PhysicWorld& _physicWorld;

private:
  Raycaster(PhysicWorld& physicWorld);
  ~Raycaster() = default;

public:
  struct RaycastParams {
    glm::vec3 from;
    glm::vec3 to;
    float radius;

    short collisionGroup;
    short collisionMask;

    enum Type {
      closest = 0,
      everything,
    };
    Type type = Type::closest;

    void* toIgnore = nullptr;

    struct ResultImpact {
      glm::vec3 impactPoint;
      glm::vec3 impactNormal;
      AbstractPhysicBody* body = nullptr;
      // PhysicBodyManager::BodyWeakRef bodyRef;
    };

    struct ResultRaw {
      bool hasHit = false;

      ResultImpact* allRawImpactsData;
      std::size_t allImpactsMaxSize;
      std::size_t allImpactsTotal;
    };

    template <std::size_t N> struct ResultArray {
      bool hasHit = false;

      std::array<ResultImpact, N> allImpactsData;
      std::size_t allImpactsTotal;
    };

    RaycastParams(const glm::vec3& rayFrom, const glm::vec3& rayTo,
                  float sweepRadius = 0.0f, short group = -1, short mask = -1,
                  Type inType = Type::closest, void* inToIgnore = nullptr)
      : from(rayFrom), to(rayTo), radius(sweepRadius), collisionGroup(group),
        collisionMask(mask), type(inType), toIgnore(inToIgnore) {}
  };

private:
  void _normalRaycast(RaycastParams& params, RaycastParams::ResultRaw& result);
  void _convexSweep(RaycastParams& params, RaycastParams::ResultRaw& result);
  bool _raycast(RaycastParams& params, RaycastParams::ResultRaw& result);

public:
  template <std::size_t N>
  bool raycast(RaycastParams& params,
               RaycastParams::ResultArray<N>& resultArray) {

    RaycastParams::ResultRaw resultRaw;
    resultRaw.allRawImpactsData = resultArray.allImpactsData.data();
    resultRaw.allImpactsMaxSize = N;
    resultRaw.allImpactsTotal = 0;
    const bool hasHit = _raycast(params, resultRaw);

    resultArray.hasHit = resultRaw.hasHit;
    resultArray.allImpactsTotal = resultRaw.allImpactsTotal;

    return hasHit;
  }
};

} // namespace physic
} // namespace gero
