
#include "headers.hpp"

TEST(physic_wrapper, query_shape_nothing) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 1.0f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  world.step(0, 0, 0);

  gero::physic::PhysicShapeDef shapeDef;
  shapeDef.type = gero::physic::PhysicShapeDef::Type::sphere;
  shapeDef.data.sphere.radius = 1.0f;

  gero::physic::QueryShape::QueryShapeParams params(glm::vec3(1000, 0, 0),
                                                     shapeDef);
  params.collisionGroup = -1;
  params.collisionMask = -1;
  gero::physic::QueryShape::QueryShapeParams::ResultArray<5> result;
  world.getQueryShape().queryShape(params, result);

  EXPECT_EQ(result.hasHit, false);
  EXPECT_EQ(result.allBodiesTotal, 0);

#if 1
  EXPECT_EQ(result.allBodiesData.size(), 5);
#else
  EXPECT_EQ(result.allBodiesRefs.size(), 5);
#endif
}

TEST(physic_wrapper, query_shape_static_object) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 1.0f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  world.step(0, 0, 0);

  gero::physic::PhysicShapeDef shapeDef;
  shapeDef.type = gero::physic::PhysicShapeDef::Type::sphere;
  shapeDef.data.sphere.radius = 5.0f;

  gero::physic::QueryShape::QueryShapeParams params(glm::vec3(5, 0, 0),
                                                     shapeDef);
  params.collisionGroup = -1;
  params.collisionMask = -1;
  gero::physic::QueryShape::QueryShapeParams::ResultArray<5> result;
  world.getQueryShape().queryShape(params, result);

  EXPECT_EQ(result.hasHit, true);
  EXPECT_EQ(result.allBodiesTotal, 1);

#if 1
  EXPECT_EQ(result.allBodiesData.size(), 5);
  EXPECT_EQ(result.allBodiesData[0], bodyRef.get());
#else
  EXPECT_EQ(result.allBodiesRefs.size(), 5);
  EXPECT_EQ(result.allBodiesRefs[0], bodyRef);
  EXPECT_EQ(result.allBodiesRefs[0].get(), bodyRef.get());
#endif
}

TEST(physic_wrapper, query_shape_all_static_objects) {
  gero::physic::PhysicWorld world;

  std::vector<gero::physic::PhysicBodyManager::BodyWeakRef> allBodyRef;
  allBodyRef.reserve(10);

  std::array<glm::vec3, 7> allBodyPos = {{
    glm::vec3(0, 0, 0),

    glm::vec3(+2, 0, 0),
    glm::vec3(0, +2, 0),
    glm::vec3(0, 0, +2),

    glm::vec3(-2, 0, 0),
    glm::vec3(0, -2, 0),
    glm::vec3(0, 0, -2),
  }};

  for (const glm::vec3 pos : allBodyPos) {
    gero::physic::PhysicBodyDef bodyDef;
    bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
    bodyDef.shape.data.sphere.radius = 1.0f;
    bodyDef.mass = 0.0f;
    auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
    bodyRef->setPosition(pos);
    world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

    allBodyRef.push_back(bodyRef);
  }

  world.step(0, 0, 0);

  gero::physic::PhysicShapeDef shapeDef;
  shapeDef.type = gero::physic::PhysicShapeDef::Type::sphere;
  shapeDef.data.sphere.radius = 5.0f;

  gero::physic::QueryShape::QueryShapeParams params(glm::vec3(0, 0, 0),
                                                     shapeDef);
  params.collisionGroup = -1;
  params.collisionMask = -1;
  gero::physic::QueryShape::QueryShapeParams::ResultArray<10> result;
  world.getQueryShape().queryShape(params, result);

  EXPECT_EQ(result.hasHit, true);
  EXPECT_EQ(result.allBodiesTotal, 7);

#if 1

  auto findBody =
    [&allBodyRef](const gero::physic::AbstractPhysicBody* inBody) {
      auto it = std::find_if(
        allBodyRef.begin(), allBodyRef.end(),
        [inBody](const gero::physic::PhysicBodyManager::BodyWeakRef& bodyRef) {
          return inBody == bodyRef.get();
        });
      return it != allBodyRef.end();
    };

  EXPECT_EQ(result.allBodiesData.size(), 10);
  EXPECT_EQ(findBody(result.allBodiesData.at(0)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(1)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(2)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(3)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(4)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(5)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(6)), true);
#else

  auto findBody =
    [&allBodyRef](
      const gero::physic::PhysicBodyManager::BodyWeakRef& inBodyRef) {
      auto it = std::find_if(
        allBodyRef.begin(), allBodyRef.end(),
        [inBodyRef](
          const gero::physic::PhysicBodyManager::BodyWeakRef& bodyRef) {
          return inBodyRef == bodyRef;
        });
      return it != allBodyRef.end();
    };

  EXPECT_EQ(result.allBodiesRefs.size(), 10);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(0)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(1)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(2)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(3)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(4)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(5)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(6)), true);
#endif
}

TEST(physic_wrapper, query_shape_some_static_objects) {
  gero::physic::PhysicWorld world;

  std::vector<gero::physic::PhysicBodyManager::BodyWeakRef> allBodyRef;
  allBodyRef.reserve(10);

  std::array<glm::vec3, 7> allBodyPos = {{
    glm::vec3(0, 0, 0),

    glm::vec3(+2, 0, 0),
    glm::vec3(0, +2, 0),
    glm::vec3(0, 0, +2),

    glm::vec3(-2, 0, 0),
    glm::vec3(0, -2, 0),
    glm::vec3(0, 0, -2),
  }};

  for (const glm::vec3 pos : allBodyPos) {
    gero::physic::PhysicBodyDef bodyDef;
    bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
    bodyDef.shape.data.sphere.radius = 1.0f;
    bodyDef.mass = 0.0f;
    auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
    bodyRef->setPosition(pos);
    world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

    allBodyRef.push_back(bodyRef);
  }

  world.step(0, 0, 0);

  gero::physic::PhysicShapeDef shapeDef;
  shapeDef.type = gero::physic::PhysicShapeDef::Type::sphere;
  shapeDef.data.sphere.radius = 2.0f;

  gero::physic::QueryShape::QueryShapeParams params(glm::vec3(2, 2, 0),
                                                     shapeDef);
  params.collisionGroup = -1;
  params.collisionMask = -1;
  gero::physic::QueryShape::QueryShapeParams::ResultArray<10> result;
  world.getQueryShape().queryShape(params, result);

  EXPECT_EQ(result.hasHit, true);
  EXPECT_EQ(result.allBodiesTotal, 3);

#if 1

  auto findBody =
    [&allBodyRef](const gero::physic::AbstractPhysicBody* inBody) {
      auto it = std::find_if(
        allBodyRef.begin(), allBodyRef.end(),
        [inBody](const gero::physic::PhysicBodyManager::BodyWeakRef& bodyRef) {
          return inBody == bodyRef.get();
        });
      return it != allBodyRef.end();
    };

  EXPECT_EQ(result.allBodiesData.size(), 10);
  EXPECT_EQ(findBody(result.allBodiesData.at(0)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(1)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(2)), true);
  EXPECT_EQ(findBody(result.allBodiesData.at(3)), false);
  EXPECT_EQ(findBody(result.allBodiesData.at(4)), false);
  EXPECT_EQ(findBody(result.allBodiesData.at(5)), false);
  EXPECT_EQ(findBody(result.allBodiesData.at(6)), false);
#else

  auto findBody =
    [&allBodyRef](
      const gero::physic::PhysicBodyManager::BodyWeakRef& inBodyRef) {
      auto it = std::find_if(
        allBodyRef.begin(), allBodyRef.end(),
        [inBodyRef](
          const gero::physic::PhysicBodyManager::BodyWeakRef& bodyRef) {
          return inBodyRef == bodyRef;
        });
      return it != allBodyRef.end();
    };

  EXPECT_EQ(result.allBodiesRefs.size(), 10);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(0)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(1)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(2)), true);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(3)), false);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(4)), false);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(5)), false);
  EXPECT_EQ(findBody(result.allBodiesRefs.at(6)), false);
#endif
}

TEST(physic_wrapper, query_shape_some_static_objects_with_collision_filters) {
  gero::physic::PhysicWorld world;

  const short filter1 = 0b0100;
  const short filter2 = 0b0010;

  struct TestDef {
    glm::vec3 pos;
    short filter;
  };

  const std::array<TestDef, 7> allBodyPos = {{
    {glm::vec3(0, 0, 0), filter1},

    {glm::vec3(+2, 0, 0), filter2},
    {glm::vec3(0, +2, 0), filter1},
    {glm::vec3(0, 0, +2), filter2},

    {glm::vec3(-2, 0, 0), filter2},
    {glm::vec3(0, -2, 0), filter1},
    {glm::vec3(0, 0, -2), filter2},
  }};

  const std::set<std::size_t> allExpectedIndices = {{0, 2, 5}};

  std::vector<gero::physic::PhysicBodyManager::BodyWeakRef> allBodyRef;
  allBodyRef.reserve(10);

  for (const TestDef& testDef : allBodyPos) {
    gero::physic::PhysicBodyDef bodyDef;
    bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
    bodyDef.shape.data.sphere.radius = 1.0f;
    bodyDef.mass = 0.0f;
    auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
    bodyRef->setPosition(testDef.pos);
    world.getPhysicBodyManager().addBody(bodyRef, testDef.filter,
                                         testDef.filter);

    allBodyRef.push_back(bodyRef);
  }

  world.step(0, 0, 0);

  gero::physic::PhysicShapeDef shapeDef;
  shapeDef.type = gero::physic::PhysicShapeDef::Type::sphere;
  shapeDef.data.sphere.radius = 5.0f;

  gero::physic::QueryShape::QueryShapeParams params(glm::vec3(0, 0, 0),
                                                     shapeDef);
  params.collisionGroup = filter1;
  params.collisionMask = filter1;
  gero::physic::QueryShape::QueryShapeParams::ResultArray<10> result;
  world.getQueryShape().queryShape(params, result);

  EXPECT_EQ(result.hasHit, true);
  EXPECT_EQ(result.allBodiesTotal, 3);

#if 1

  EXPECT_EQ(result.allBodiesData.size(), 10);

  int totalFound = 0;
  for (std::size_t ii = 0; ii < result.allBodiesTotal; ++ii) {
    for (std::size_t index = 0; index < allBodyRef.size(); ++index) {
      if (result.allBodiesData[ii] == allBodyRef[index].get() &&
          allExpectedIndices.count(index) > 0) {
        ++totalFound;
      }
    }
  }
#else

  EXPECT_EQ(result.allBodiesRefs.size(), 10);

  int totalFound = 0;
  for (std::size_t ii = 0; ii < result.allBodiesTotal; ++ii) {
    for (std::size_t index = 0; index < allBodyRef.size(); ++index) {
      if (result.allBodiesRefs[ii] == allBodyRef[index] &&
          allExpectedIndices.count(index) > 0) {
        ++totalFound;
      }
    }
  }
#endif

  EXPECT_EQ(totalFound, 3);
}

TEST(physic_wrapper, query_shape_some_static_objects_with_to_ignore) {
  gero::physic::PhysicWorld world;

  const std::array<glm::vec3, 7> allBodyPos = {{
    glm::vec3(0, 0, 0),

    glm::vec3(+2, 0, 0),
    glm::vec3(0, +2, 0),
    glm::vec3(0, 0, +2),

    glm::vec3(-2, 0, 0),
    glm::vec3(0, -2, 0),
    glm::vec3(0, 0, -2),
  }};

  const std::set<std::size_t> allExpectedIndices = {{0, 1, 2, 3, /*4,*/ 5, 6}};

  std::vector<gero::physic::PhysicBodyManager::BodyWeakRef> allBodyRef;
  allBodyRef.reserve(10);

  for (const glm::vec3& pos : allBodyPos) {
    gero::physic::PhysicBodyDef bodyDef;
    bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
    bodyDef.shape.data.sphere.radius = 1.0f;
    bodyDef.mass = 0.0f;
    auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
    bodyRef->setPosition(pos);
    world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

    allBodyRef.push_back(bodyRef);
  }

  world.step(0, 0, 0);

  gero::physic::PhysicShapeDef shapeDef;
  shapeDef.type = gero::physic::PhysicShapeDef::Type::sphere;
  shapeDef.data.sphere.radius = 5.0f;

  gero::physic::QueryShape::QueryShapeParams params(glm::vec3(0, 0, 0),
                                                     shapeDef);
  params.collisionGroup = -1;
  params.collisionMask = -1;
  params.toIgnore = &(*allBodyRef[4]);
  gero::physic::QueryShape::QueryShapeParams::ResultArray<10> result;
  world.getQueryShape().queryShape(params, result);

  EXPECT_EQ(result.hasHit, true);
  EXPECT_EQ(result.allBodiesTotal, 6);

#if 1

  EXPECT_EQ(result.allBodiesData.size(), 10);

  int totalFound = 0;
  for (std::size_t ii = 0; ii < result.allBodiesTotal; ++ii) {
    for (std::size_t index = 0; index < allBodyRef.size(); ++index) {
      if (result.allBodiesData[ii] == allBodyRef[index].get() &&
          allExpectedIndices.count(index) > 0) {
        ++totalFound;
      }
    }
  }
#else

  EXPECT_EQ(result.allBodiesRefs.size(), 10);

  int totalFound = 0;
  for (std::size_t ii = 0; ii < result.allBodiesTotal; ++ii) {
    for (std::size_t index = 0; index < allBodyRef.size(); ++index) {
      if (result.allBodiesRefs[ii] == allBodyRef[index] &&
          allExpectedIndices.count(index) > 0) {
        ++totalFound;
      }
    }
  }
#endif

  EXPECT_EQ(totalFound, 6);
}

#if 0

TEST(physic_wrapper, query_shape_static_object_out_of_scope) {
  gero::physic::PhysicWorld world;

  gero::physic::PhysicBodyDef bodyDef;
  bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
  bodyDef.shape.data.sphere.radius = 1.0f;
  bodyDef.mass = 0.0f;
  auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
  bodyRef->setPosition({0, 0, 0});
  world.getPhysicBodyManager().addBody(bodyRef, -1, -1);

  world.step(0, 0, 0);

  {
    gero::physic::QueryShape::QueryShapeParams params(glm::vec3(5, 0, 0), 5.0f);
    params.collisionGroup = -1;
    params.collisionMask = -1;
    gero::physic::QueryShape::QueryShapeParams::ResultArray<5> result;
    world.getQueryShape().queryShape(params, result);

    EXPECT_EQ(result.hasHit, true);
    EXPECT_EQ(result.allBodiesTotal, 1);
    EXPECT_EQ(result.allBodiesRefs.size(), 5);
    EXPECT_EQ(result.allBodiesRefs[0], bodyRef);
  }

  {
    gero::physic::QueryShape::QueryShapeParams params(glm::vec3(5, 0, 0), 5.0f);
    params.collisionGroup = -1;
    params.collisionMask = -1;
    gero::physic::QueryShape::QueryShapeParams::ResultArray<5> result;
    world.getQueryShape().queryShape(params, result);

    EXPECT_EQ(result.hasHit, true);
    EXPECT_EQ(result.allBodiesTotal, 1);
    EXPECT_EQ(result.allBodiesRefs.size(), 5);
    EXPECT_EQ(result.allBodiesRefs[0], bodyRef);
  }

  {
    gero::physic::QueryShape::QueryShapeParams params(glm::vec3(5, 0, 0), 5.0f);
    params.collisionGroup = -1;
    params.collisionMask = -1;
    gero::physic::QueryShape::QueryShapeParams::ResultArray<5> result;
    world.getQueryShape().queryShape(params, result);

    EXPECT_EQ(result.hasHit, true);
    EXPECT_EQ(result.allBodiesTotal, 1);
    EXPECT_EQ(result.allBodiesRefs.size(), 5);
    EXPECT_EQ(result.allBodiesRefs[0], bodyRef);
  }
}

#endif
