
#include "headers.hpp"

TEST(physic_wrapper, test_all_shapes) {
  std::array<glm::vec3, 4> k_rawVertices = {{
    glm::vec3(0, 0, 0),
    glm::vec3(1, 0, 0),
    glm::vec3(1, 1, 0),
    glm::vec3(0, 1, 0),
  }};
  std::array<int32_t, 6> k_rawIndices = {{1, 2, 3, 1, 0, 3}};

  {

    gero::physic::PhysicWorld world;

    const float frameTime = 1.0f / 60.0f;

    {
      gero::physic::PhysicBodyDef bodyDef;
      bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
      bodyDef.shape.data.sphere.radius = 0.5f;
      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      gero::physic::PhysicBodyDef bodyDef;
      bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::box;
      bodyDef.shape.data.box.size = {0.5f, 0.5f, 0.5f};
      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      gero::physic::PhysicBodyDef bodyDef;
      bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::capsule;
      bodyDef.shape.data.capsule.radius = 0.5f;
      bodyDef.shape.data.capsule.height = 1.0f;
      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      gero::physic::PhysicBodyDef bodyDef;

      bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::staticMesh;
      bodyDef.shape.data.staticMesh.verticesData = k_rawVertices.data();
      bodyDef.shape.data.staticMesh.verticesLength = k_rawVertices.size();
      bodyDef.shape.data.staticMesh.indicesData = k_rawIndices.data();
      bodyDef.shape.data.staticMesh.indicesLength = k_rawIndices.size();

      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

    {
      gero::physic::PhysicBodyDef bodyDef;
      bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::compound;

      {
        auto shapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
        shapeDef->type = gero::physic::PhysicShapeDef::Type::sphere;
        shapeDef->data.sphere.radius = 0.5f;
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
        shapeDef->type = gero::physic::PhysicShapeDef::Type::box;
        shapeDef->data.box.size = {0.5f, 0.5f, 0.5f};
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
        shapeDef->type = gero::physic::PhysicShapeDef::Type::capsule;
        shapeDef->data.capsule.radius = 0.5f;
        shapeDef->data.capsule.height = 1.0f;
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
        shapeDef->type = gero::physic::PhysicShapeDef::Type::capsule;
        shapeDef->data.capsule.radius = 0.5f;
        shapeDef->data.capsule.height = 1.0f;
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
        shapeDef->type = gero::physic::PhysicShapeDef::Type::staticMesh;
        shapeDef->data.staticMesh.verticesData = k_rawVertices.data();
        shapeDef->data.staticMesh.verticesLength = k_rawVertices.size();
        shapeDef->data.staticMesh.indicesData = k_rawIndices.data();
        shapeDef->data.staticMesh.indicesLength = k_rawIndices.size();
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }
      {
        auto shapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
        shapeDef->type = gero::physic::PhysicShapeDef::Type::compound;
        {
          auto subShapeDef = std::make_shared<gero::physic::PhysicShapeDef>();
          subShapeDef->type = gero::physic::PhysicShapeDef::Type::sphere;
          subShapeDef->data.sphere.radius = 0.5f;
          shapeDef->data.compound.childShapes.push_back(
            {glm::identity<glm::mat4>(), subShapeDef});
        }
        bodyDef.shape.data.compound.childShapes.push_back(
          {glm::identity<glm::mat4>(), shapeDef});
      }

      bodyDef.mass = 0.0f;
      bodyDef.mask = 1;
      bodyDef.group = 1;
      auto bodyRef = world.getPhysicBodyManager().createBody(bodyDef);
      bodyRef->setPosition({0, 0, 10});
      // bodyRef->setUserValue(1111);
      world.getPhysicBodyManager().addBody(bodyRef, bodyDef.group,
                                           bodyDef.mask);
    }

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
  }
}
