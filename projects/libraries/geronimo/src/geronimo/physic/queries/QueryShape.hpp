
#pragma once

#include "geronimo/helpers/GLMath.hpp"
#include "geronimo/physic/body/PhysicBodyManager.hpp"
#include "geronimo/physic/shape/PhysicShapeDef.hpp"

#include <array>

namespace gero {
namespace physic {

class PhysicWorld;

class QueryShape {
  friend PhysicWorld;

private:
  PhysicWorld& _physicWorld;

private:
  QueryShape(PhysicWorld& physicWorld);
  ~QueryShape() = default;

public:
  struct QueryShapeParams {
    glm::vec3 position;

    // float radius;
    PhysicShapeDef shape;

    short collisionGroup;
    short collisionMask;

    void* toIgnore = nullptr;

    struct ResultRaw {
      bool hasHit = false;

      // PhysicBodyManager::BodyWeakRef* allRawBodiesRefs;
      AbstractPhysicBody** allRawBodiesData; // abstract
      std::size_t allBodiesMaxSize;
      std::size_t allBodiesTotal;
    };

    template <std::size_t N> struct ResultArray {
      bool hasHit = false;

      std::array<AbstractPhysicBody*, N> allBodiesData;
      // std::array<PhysicBodyManager::BodyWeakRef, N> allBodiesRefs;
      std::size_t allBodiesTotal;
    };

    QueryShapeParams(const glm::vec3& inPosition, PhysicShapeDef inShape,
                     short inGroup = -1, short inMask = -1,
                     void* inToIgnore = nullptr)
      : position(inPosition), shape(inShape), collisionGroup(inGroup),
        collisionMask(inMask), toIgnore(inToIgnore) {}
  };

private:
  bool _queryShape(QueryShapeParams& inParams,
                   QueryShapeParams::ResultRaw& outResultArray);

public:
  template <std::size_t N>
  bool queryShape(QueryShapeParams& inParams,
                  QueryShapeParams::ResultArray<N>& outResultArray) {
    QueryShapeParams::ResultRaw resultRaw;
    resultRaw.allRawBodiesData = outResultArray.allBodiesData.data();
    // resultRaw.allRawBodiesRefs = outResultArray.allBodiesRefs.data();
    resultRaw.allBodiesMaxSize = N;
    resultRaw.allBodiesTotal = 0;
    const bool hasHit = _queryShape(inParams, resultRaw);

    outResultArray.hasHit = resultRaw.hasHit;
    outResultArray.allBodiesTotal = resultRaw.allBodiesTotal;

    return hasHit;
  }
};

} // namespace physic
} // namespace gero
