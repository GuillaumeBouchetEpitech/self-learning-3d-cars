
#include "headers.hpp"

TEST(physic_wrapper, raycast_nothing) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 1.0f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  world.step(0, 0, 0);

  gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(1000, 0, 10),
                                                    glm::vec3(1000, 0, -10));
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = -1;
  paramsRay.collisionMask = -1;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::closest;
  paramsRay.toIgnore = nullptr;

  gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  world.getRaycaster().raycast(paramsSphere, resultSphere);

  EXPECT_EQ(resultRay.hasHit, false);
  EXPECT_EQ(resultRay.allImpactsTotal, 0);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);

  EXPECT_EQ(resultSphere.hasHit, false);
  EXPECT_EQ(resultSphere.allImpactsTotal, 0);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
}

TEST(physic_wrapper, raycast_closest_static_object) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 1.0f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  world.step(0, 0, 0);

  gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(0, 0, 10),
                                                    glm::vec3(0, 0, -10));
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = -1;
  paramsRay.collisionMask = -1;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::closest;
  paramsRay.toIgnore = nullptr;

  gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
#else
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
#endif
}

TEST(physic_wrapper, raycast_closest_static_object_with_collision_filters) {
  gero::physic::PhysicWorld world;

  const short filter1 = 0b0100;
  const short filter2 = 0b0010;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 1.0f;
  bodyDef_1.mass = 0.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 5});
  world.getPhysicBodyManager().addBody(bodyRef_1, filter1, filter1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_2.shape.data.sphere.radius = 1.0f;
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, filter2, filter2);

  world.step(0, 0, 0);

  gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(0, 0, 10),
                                                    glm::vec3(0, 0, -10));
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = filter2;
  paramsRay.collisionMask = filter2;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::closest;
  paramsRay.toIgnore = nullptr;

  gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef_2.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef_2.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
#else
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef_2);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef_2.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef_2);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef_2.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
#endif
}

TEST(physic_wrapper, raycast_closest_static_object_with_ignore_parameter) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 1.0f;
  bodyDef_1.mass = 0.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 5});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_2.shape.data.sphere.radius = 1.0f;
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  world.step(0, 0, 0);

  gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(0, 0, 10),
                                                    glm::vec3(0, 0, -10));
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = -1;
  paramsRay.collisionMask = -1;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::closest;
  paramsRay.toIgnore = &(*bodyRef_1);

  gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef_2.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef_2.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
#else
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef_2);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef_2.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef_2);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef_2.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
#endif
}

TEST(physic_wrapper, raycast_every_static_objects) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 1.0f;
  bodyDef_1.mass = 0.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 5});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_2.shape.data.sphere.radius = 1.0f;
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  world.step(0, 0, 0);

  gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(0, 0, 10),
                                                    glm::vec3(0, 0, -10));
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = -1;
  paramsRay.collisionMask = -1;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::everything;
  // paramsRay.toIgnore = &(*bodyRef_1);

  gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 2);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef_2.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);
  EXPECT_EQ(resultRay.allImpactsData[1].body, bodyRef_1.get());
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 2);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef_2.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
  EXPECT_EQ(resultSphere.allImpactsData[1].body, bodyRef_1.get());
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#else

  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 2);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef_2);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef_2.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 1, 0.001);
  EXPECT_EQ(resultRay.allImpactsData[1].bodyRef, bodyRef_1);
  EXPECT_EQ(resultRay.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 2);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef_2);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef_2.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 1, 0.001);
  EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef, bodyRef_1);
  EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#endif
}

TEST(physic_wrapper, raycast_every_static_objects_with_ignore_parameter) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 1.0f;
  bodyDef_1.mass = 0.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 5});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_2.shape.data.sphere.radius = 1.0f;
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_3;
  bodyDef_3.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_3.shape.data.sphere.radius = 1.0f;
  bodyDef_3.mass = 0.0f;
  auto bodyRef_3 = world.getPhysicBodyManager().createBody(bodyDef_3);
  bodyRef_3->setPosition({0, 0, -5});
  world.getPhysicBodyManager().addBody(bodyRef_3, -1, -1);

  world.step(0, 0, 0);

  gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(0, 0, 10),
                                                    glm::vec3(0, 0, -10));
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = -1;
  paramsRay.collisionMask = -1;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::everything;
  paramsRay.toIgnore = &(*bodyRef_2);

  gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 2);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef_3.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, -4, 0.001);
  EXPECT_EQ(resultRay.allImpactsData[1].body, bodyRef_1.get());
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 2);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef_3.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, -4, 0.001);
  EXPECT_EQ(resultSphere.allImpactsData[1].body, bodyRef_1.get());
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#else
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 2);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef_3);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef_3.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, -4, 0.001);
  EXPECT_EQ(resultRay.allImpactsData[1].bodyRef, bodyRef_1);
  EXPECT_EQ(resultRay.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 2);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef_3);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef_3.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, -4, 0.001);
  EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef, bodyRef_1);
  EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#endif
}

TEST(physic_wrapper, raycast_every_static_objects_being_looped) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 1.0f;
  bodyDef_1.mass = 0.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 5});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_2.shape.data.sphere.radius = 1.0f;
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_3;
  bodyDef_3.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_3.shape.data.sphere.radius = 1.0f;
  bodyDef_3.mass = 0.0f;
  auto bodyRef_3 = world.getPhysicBodyManager().createBody(bodyDef_3);
  bodyRef_3->setPosition({0, 0, -5});
  world.getPhysicBodyManager().addBody(bodyRef_3, -1, -1);

  world.step(0, 0, 0);

  for (int ii = 0; ii < 10; ++ii) {

    gero::physic::Raycaster::RaycastParams paramsRay(glm::vec3(0, 0, 10),
                                                      glm::vec3(0, 0, -10));
    paramsRay.radius = 0.0f;
    paramsRay.collisionGroup = -1;
    paramsRay.collisionMask = -1;
    paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::everything;
    paramsRay.toIgnore = &(*bodyRef_2);

    gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
    paramsSphere.radius = 1.0f;

    gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
    world.getRaycaster().raycast(paramsRay, resultRay);

    gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
    world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
    EXPECT_EQ(resultRay.hasHit, true);
    EXPECT_EQ(resultRay.allImpactsTotal, 2);
    EXPECT_EQ(resultRay.allImpactsData.size(), 5);
    EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef_3.get());
    EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, -4, 0.001);
    EXPECT_EQ(resultRay.allImpactsData[1].body, bodyRef_1.get());
    EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

    EXPECT_EQ(resultSphere.hasHit, true);
    EXPECT_EQ(resultSphere.allImpactsTotal, 2);
    EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
    EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef_3.get());
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, -4, 0.001);
    EXPECT_EQ(resultSphere.allImpactsData[1].body, bodyRef_1.get());
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#else
    EXPECT_EQ(resultRay.hasHit, true);
    EXPECT_EQ(resultRay.allImpactsTotal, 2);
    EXPECT_EQ(resultRay.allImpactsData.size(), 5);
    EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef_3);
    EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef_3.get());
    EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, -4, 0.001);
    EXPECT_EQ(resultRay.allImpactsData[1].bodyRef, bodyRef_1);
    EXPECT_EQ(resultRay.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
    EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

    EXPECT_EQ(resultSphere.hasHit, true);
    EXPECT_EQ(resultSphere.allImpactsTotal, 2);
    EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
    EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef_3);
    EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef_3.get());
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, -4, 0.001);
    EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef, bodyRef_1);
    EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
    EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#endif
  }
}

TEST(physic_wrapper, raycast_repro_static_box_bug) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::box;
  bodyDef_1.shape.data.box.size = {1, 1, 1};
  bodyDef_1.mass = 0.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  // gero::physic::PhysicBodyDef bodyDef_2;
  // bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  // bodyDef_2.shape.data.sphere.radius = 1.0f;
  // bodyDef_2.mass = 0.0f;
  // auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  // bodyRef_2->setPosition({0, 0, 0});
  // world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  // gero::physic::PhysicBodyDef bodyDef_3;
  // bodyDef_3.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  // bodyDef_3.shape.data.sphere.radius = 1.0f;
  // bodyDef_3.mass = 0.0f;
  // auto bodyRef_3 = world.getPhysicBodyManager().createBody(bodyDef_3);
  // bodyRef_3->setPosition({0, 0, -5});
  // world.getPhysicBodyManager().addBody(bodyRef_3, -1, -1);

  world.step(0, 0, 0);

  const glm::vec3 rayFrom = glm::vec3(10.5f, 10.0f, 0);
  const glm::vec3 rayDir = -glm::vec3(20.0f, 20.0f, 0);
  const glm::vec3 rayTo = rayFrom + rayDir;
  // const float angle = std::atan2(rayDir.y, rayDir.x);

  gero::physic::Raycaster::RaycastParams paramsRay(rayFrom, rayTo);
  paramsRay.radius = 0.0f;
  paramsRay.collisionGroup = -1;
  paramsRay.collisionMask = -1;
  paramsRay.type = gero::physic::Raycaster::RaycastParams::Type::closest;
  // paramsRay.toIgnore = &(*bodyRef_2);

  // gero::physic::Raycaster::RaycastParams paramsSphere = paramsRay;
  // paramsSphere.radius = 1.0f;

  gero::physic::Raycaster::RaycastParams::ResultArray<5> resultRay;
  world.getRaycaster().raycast(paramsRay, resultRay);

  // gero::physic::Raycaster::RaycastParams::ResultArray<5> resultSphere;
  // world.getRaycaster().raycast(paramsSphere, resultSphere);

#if 1
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 1);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].body, bodyRef_1.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 1.0f, 0.1f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0.0f, 0.1f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 0, 0.1f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0.5f, 0.1f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0.0f, 0.1f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, 0, 0.1f);

  // EXPECT_EQ(resultSphere.hasHit, true);
  // EXPECT_EQ(resultSphere.allImpactsTotal, 1);
  // EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  // EXPECT_EQ(resultSphere.allImpactsData[0].body, bodyRef_1.get());
  // // EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x,
  // std::cos(angle), 0.1f);
  // // EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y,
  // std::sin(angle), 0.1f);
  // EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 1.0f, 0.1f);
  // EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0.4f, 0.1f);
  // EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 0, 0.1f);
  // EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0.5f, 0.1f);
  // EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0.5f, 0.1f);
  // EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, 0, 0.1f);
#else
  EXPECT_EQ(resultRay.hasHit, true);
  EXPECT_EQ(resultRay.allImpactsTotal, 2);
  EXPECT_EQ(resultRay.allImpactsData.size(), 5);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef, bodyRef_3);
  EXPECT_EQ(resultRay.allImpactsData[0].bodyRef.get(), bodyRef_3.get());
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[0].impactPoint.z, -4, 0.001);
  EXPECT_EQ(resultRay.allImpactsData[1].bodyRef, bodyRef_1);
  EXPECT_EQ(resultRay.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultRay.allImpactsData[1].impactPoint.z, 6, 0.001);

  EXPECT_EQ(resultSphere.hasHit, true);
  EXPECT_EQ(resultSphere.allImpactsTotal, 2);
  EXPECT_EQ(resultSphere.allImpactsData.size(), 5);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef, bodyRef_3);
  EXPECT_EQ(resultSphere.allImpactsData[0].bodyRef.get(), bodyRef_3.get());
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[0].impactPoint.z, -4, 0.001);
  EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef, bodyRef_1);
  EXPECT_EQ(resultSphere.allImpactsData[1].bodyRef.get(), bodyRef_1.get());
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactNormal.z, 1, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.x, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.y, 0, 0.001f);
  EXPECT_NEAR(resultSphere.allImpactsData[1].impactPoint.z, 6, 0.001);
#endif
}
