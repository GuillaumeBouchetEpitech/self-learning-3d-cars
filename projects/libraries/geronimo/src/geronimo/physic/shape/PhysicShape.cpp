
#include "PhysicShape.hpp"

#include "geronimo/physic/helpers/transformConversion.hpp"

#include "geronimo/helpers/internals/BulletPhysics.hpp"
#include "geronimo/system/ErrorHandler.hpp"
#include "geronimo/system/TraceLogger.hpp"

namespace gero {
namespace physic {

PhysicShape::PhysicShape(const PhysicShapeDef& def) : _type(def.type) {}

PhysicShape::~PhysicShape() { delete _bullet.shape, _bullet.shape = nullptr; }

PhysicShape* PhysicShape::create(const PhysicShapeDef& def, bool isDynamic) {
  switch (def.type) {
  case PhysicShapeDef::Type::sphere:
    return new SphereShape(def);
  case PhysicShapeDef::Type::capsule:
    return new CapsuleShape(def);
  case PhysicShapeDef::Type::box:
    return new BoxShape(def);
  case PhysicShapeDef::Type::staticMesh:
    return new StaticMeshShape(def, isDynamic);
  case PhysicShapeDef::Type::compound:
    return new CompoundShape(def, isDynamic);
  default:
    D_THROW(std::runtime_error, "unknown physic shape type");
  }
}

btCollisionShape* PhysicShape::getRawShape() { return _bullet.shape; }

//
//
//

CapsuleShape::CapsuleShape(const PhysicShapeDef& def) : PhysicShape(def) {
  const auto& data = def.data.capsule;
  _bullet.shape = new btCapsuleShapeZ(data.radius, data.height);
}

//
//
//

BoxShape::BoxShape(const PhysicShapeDef& def) : PhysicShape(def) {
  const glm::vec3& size = def.data.box.size;
  _bullet.shape =
    new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));
}

//
//
//

SphereShape::SphereShape(const PhysicShapeDef& def) : PhysicShape(def) {
  _bullet.shape = new btSphereShape(def.data.sphere.radius);
}

//
//
//

StaticMeshShape::StaticMeshShape(const PhysicShapeDef& def, bool isDynamic)
  : PhysicShape(def) {
  if (isDynamic)
    D_THROW(std::runtime_error, "physic staticMesh cannot have non zero mass");

  const auto& data = def.data.staticMesh;

  const int32_t totalDecimals = int32_t(data.verticesLength) * 3;
  _verticesData = std::make_unique<btScalar[]>(std::size_t(totalDecimals));
  btScalar* verticesDataRaw = _verticesData.get();
  // const uint32_t verticesSizeInBytes = uint32_t(data.verticesLength) *
  // uint32_t(sizeof(glm::vec3)); std::memcpy(_verticesData.get(),
  // data.verticesData, verticesSizeInBytes);

  for (std::size_t ii = 0; ii < data.verticesLength; ++ii) {
    verticesDataRaw[ii * 3 + 0] = btScalar(data.verticesData[ii].x);
    verticesDataRaw[ii * 3 + 1] = btScalar(data.verticesData[ii].y);
    verticesDataRaw[ii * 3 + 2] = btScalar(data.verticesData[ii].z);
  }

  const int verticesStride = sizeof(btScalar) * 3;

  const int32_t triangleNumber = int32_t(data.indicesLength) / 3;
  _indicesData = std::make_unique<int32_t[]>(std::size_t(data.indicesLength));
  int32_t* indicesDataRaw = _indicesData.get();
  const uint32_t indicesSizeInBytes =
    uint32_t(data.indicesLength) * uint32_t(sizeof(int));
  std::memcpy(indicesDataRaw, data.indicesData, indicesSizeInBytes);
  const int indicesStride = 3 * sizeof(int);

  _indexVertexArrays = new btTriangleIndexVertexArray(
    triangleNumber, indicesDataRaw, indicesStride, totalDecimals,
    verticesDataRaw, verticesStride);

  constexpr bool useQuantizedAabbCompression = false;
  _bullet.shape =
    new btBvhTriangleMeshShape(_indexVertexArrays, useQuantizedAabbCompression);
}

StaticMeshShape::~StaticMeshShape() {
  delete _indexVertexArrays, _indexVertexArrays = nullptr;
}

//
//
//

CompoundShape::CompoundShape(const PhysicShapeDef& def, bool isDynamic)
  : PhysicShape(def) {
  const auto& data = def.data.compound;

  btCompoundShape* compoundShape = new btCompoundShape();

  btTransform localTrans;

  btScalar tmpMat[16];

  _children.reserve(data.childShapes.size());
  for (auto& shapeDef : data.childShapes) {
    localTrans.setIdentity();

    // localTrans.setFromOpenGLMatrix(glm::value_ptr(shapeDef.transform));
    setBulletFromGlm(shapeDef.transform, tmpMat);
    localTrans.setFromOpenGLMatrix(tmpMat);

    PhysicShape* childShape = PhysicShape::create(*shapeDef.shape, isDynamic);
    _children.push_back(childShape);

    compoundShape->addChildShape(localTrans, childShape->getRawShape());
  }

  _bullet.shape = compoundShape;
}

CompoundShape::~CompoundShape() {
  delete _bullet.shape, _bullet.shape = nullptr;
  for (PhysicShape* childShape : _children)
    delete childShape;
  _children.clear();
}

} // namespace physic
} // namespace gero
