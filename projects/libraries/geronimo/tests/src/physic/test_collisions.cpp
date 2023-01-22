
#include "headers.hpp"

TEST(physic_wrapper, dynamic_object_is_affected_by_gravity) {
  gero::physic::PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 0.5f;
  bodyDef.mass = 1.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 10});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_LT(bodyRef->getPosition().z, 0);
}

TEST(physic_wrapper, static_object_is_not_affected_by_gravity) {
  gero::physic::PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 0.5f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 10});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_EQ(bodyRef->getPosition().z, 10);
}

TEST(physic_wrapper, dynamic_object_can_collide_with_static_object) {
  gero::physic::PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 0.5f;
  bodyDef_1.mass = 1.0f;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 10});
  world.getPhysicBodyManager().addBody(bodyRef_1, -1, -1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::box;
  bodyDef_2.shape.data.box.size = {20, 20, 0.5f};
  bodyDef_2.mass = 0.0f;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef_2, -1, -1);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_LT(bodyRef_1->getPosition().z, 10);
  EXPECT_GT(bodyRef_1->getPosition().z, 0);
}

TEST(
  physic_wrapper,
  dynamic_object_cannot_collide_with_static_object_if_set_with_non_matching_collision_filter) {
  gero::physic::PhysicWorld world;

  const float frameTime = 1.0f / 60.0f;

  gero::physic::PhysicBodyDef bodyDef_1;
  bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef_1.shape.data.sphere.radius = 0.5f;
  bodyDef_1.mass = 1.0f;
  bodyDef_1.mask = 1;
  bodyDef_1.group = 1;
  auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
  bodyRef_1->setPosition({0, 0, 10});
  bodyRef_1->setUserValue(1111);
  world.getPhysicBodyManager().addBody(bodyRef_1, 1, 1);

  gero::physic::PhysicBodyDef bodyDef_2;
  bodyDef_2.shape.type = gero::physic::PhysicShapeDef::Type::box;
  bodyDef_2.shape.data.box.size = {20, 20, 0.5f};
  bodyDef_2.mass = 0.0f;
  bodyDef_2.mask = 8;
  bodyDef_2.group = 8;
  auto bodyRef_2 = world.getPhysicBodyManager().createBody(bodyDef_2);
  bodyRef_2->setPosition({0, 0, 0});
  bodyRef_2->setUserValue(2222);
  world.getPhysicBodyManager().addBody(bodyRef_2, 8, 8);

  for (int ii = 0; ii < 100; ++ii)
    world.step(frameTime, 1, frameTime);

  EXPECT_LT(bodyRef_1->getPosition().z, 0);
}

// TEST(
//   physic_wrapper,
//   dump_data) {
//   gero::physic::PhysicWorld world;

//   // const float frameTime = 1.0f / 60.0f;

//   std::vector<PhysicBodyManager::BodyWeakRef> allRefs;
//   allRefs.reserve(64);

//   for (int ii = 0; ii < 10; ++ii)
//   {

//     gero::physic::PhysicBodyDef bodyDef_1;
//     bodyDef_1.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
//     bodyDef_1.shape.data.sphere.radius = 0.5f;
//     bodyDef_1.mass = 1.0f;
//     bodyDef_1.mask = 1;
//     bodyDef_1.group = 1;
//     auto bodyRef_1 = world.getPhysicBodyManager().createBody(bodyDef_1);
//     bodyRef_1->setPosition({ii * 1, 0, 10});
//     bodyRef_1->setUserValue(ii);

//     world.getPhysicBodyManager().addBody(bodyRef_1, 1, 1);

//     allRefs.push_back(bodyRef_1);
//   }

//   EXPECT_EQ(world.getPhysicBodyManager().size(), 10);

//   world.step(0, 1, 0);
//   // for (int ii = 0; ii < 100; ++ii)
//   //   world.step(frameTime, 1, frameTime);

//   // EXPECT_LT(bodyRef_1->getPosition().z, 0);

//   D_MYERR("###");
//   D_MYERR("###");
//   D_MYERR("###");

//   world.getPhysicBodyManager().dumpData();

//   D_MYERR("###");
//   D_MYERR("###");
//   D_MYERR("###");

//   world.getPhysicBodyManager().destroyBody(allRefs[4]);
//   world.getPhysicBodyManager().destroyBody(allRefs[3]);
//   world.getPhysicBodyManager().destroyBody(allRefs[6]);

//   world.getPhysicBodyManager().dumpData();

//   D_MYERR("###");
//   D_MYERR("###");
//   D_MYERR("###");

//   EXPECT_EQ(world.getPhysicBodyManager().size(), 7);
// }
