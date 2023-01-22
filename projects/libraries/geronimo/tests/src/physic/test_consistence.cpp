
#include "headers.hpp"

#include "geronimo/system/containers/weak_ref_data_pool.hpp"

TEST(physic_wrapper, test_consistence_raw) {
  gero::physic::PhysicWorld world;
  auto& bodyManager = world.getPhysicBodyManager();

  auto addBody = [&bodyManager](int userVal) {
    gero::physic::PhysicBodyDef bodyDef;
    bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
    bodyDef.shape.data.sphere.radius = 1.0f;
    bodyDef.mass = 0.0f;
    auto bodyRef = bodyManager.createBody(bodyDef);

    bodyManager.addBody(bodyRef, -1, -1);

    bodyRef->setUserValue(userVal);

    return bodyRef;
  };

  auto ref0 = addBody(0);
  auto ref1 = addBody(1);
  auto ref2 = addBody(2);
  auto ref3 = addBody(3);
  auto ref4 = addBody(4);

  world.step(0, 0, 0);

  struct TestData {
    int expectedVal;
    gero::physic::PhysicBodyManager::BodyWeakRef* pRef;
  };

  {

    std::array<TestData, 5> expectedVal = {{
      {0, &ref0},
      {1, &ref1},
      {2, &ref2},
      {3, &ref3},
      {4, &ref4},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.index(), index)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  bodyManager.destroyBody(bodyManager.getBody(1));

  {

    std::array<TestData, 4> expectedVal = {{
      {0, &ref0},
      {4, &ref4},
      {2, &ref2},
      {3, &ref3},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.index(), index)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  auto ref5 = addBody(5);

  {

    std::array<TestData, 5> expectedVal = {{
      {0, &ref0},
      {4, &ref4},
      {2, &ref2},
      {3, &ref3},
      {5, &ref5},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.index(), index)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  ref2 = std::move(ref4);

  {

    std::array<TestData, 5> expectedVal = {{
      {0, &ref0},
      {4, &ref2},
      {2, &ref4},
      {3, &ref3},
      {5, &ref5},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.index(), index)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  std::swap(ref2, ref3);

  {

    std::array<TestData, 5> expectedVal = {{
      {0, &ref0},
      {4, &ref3},
      {2, &ref4},
      {3, &ref2},
      {5, &ref5},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.index(), index)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }
}

TEST(physic_wrapper, test_consistence_nested_class_attribute) {
  gero::physic::PhysicWorld world;
  auto& bodyManager = world.getPhysicBodyManager();

  struct TestClass {

    gero::physic::PhysicWorld& _world;

    int _val;
    gero::physic::PhysicBodyManager::BodyWeakRef _bodyRef;

    TestClass(gero::physic::PhysicWorld& world, int val) : _world(world) {
      _val = val;

      //
      //

      auto& bodyManager = _world.getPhysicBodyManager();

      gero::physic::PhysicBodyDef bodyDef;
      bodyDef.shape.type = gero::physic::PhysicShapeDef::Type::sphere;
      bodyDef.shape.data.sphere.radius = 1.0f;
      bodyDef.mass = 0.0f;
      _bodyRef = bodyManager.createBody(bodyDef);

      bodyManager.addBody(_bodyRef, -1, -1);

      _bodyRef->setUserValue(_val);
    }

    TestClass(const TestClass& other) = delete;
    TestClass(TestClass&& other) : _world(other._world) {
      if (&other == this)
        return;
      std::swap(_val, other._val);
      std::swap(_bodyRef, other._bodyRef);
    }

    TestClass& operator=(const TestClass& other) = delete;
    TestClass& operator=(TestClass&& other) {
      if (&other == this)
        return *this;
      std::swap(_val, other._val);
      std::swap(_bodyRef, other._bodyRef);
      return *this;
    }

    ~TestClass() {
      if (_bodyRef)
        _world.getPhysicBodyManager().destroyBody(_bodyRef);
    }
  };

  using DataPoll = gero::weak_ref_data_pool<TestClass, TestClass, 64>;
  using DataRef = DataPoll::weak_ref;

  DataPoll dataPool;

  DataRef ref0 = dataPool.acquire(world, 111);
  DataRef ref1 = dataPool.acquire(world, 222);
  DataRef ref2 = dataPool.acquire(world, 333);
  DataRef ref3 = dataPool.acquire(world, 444);
  DataRef ref4 = dataPool.acquire(world, 555);

  world.step(0, 0, 0);

  struct TestData {
    int expectedVal;
    DataRef* pRef;
  };

  {

    std::array<TestData, 5> expectedVal = {{
      {111, &ref0},
      {222, &ref1},
      {333, &ref2},
      {444, &ref3},
      {555, &ref4},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.index(), index)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_val, currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  dataPool.release(dataPool.get(1));

  {

    std::array<TestData, 4> expectedVal = {{
      {111, &ref0},
      {555, &ref4},
      {333, &ref2},
      {444, &ref3},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_val, currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  DataRef ref5 = dataPool.acquire(world, 666);

  {

    std::array<TestData, 5> expectedVal = {{
      {111, &ref0},
      {555, &ref4},
      {333, &ref2},
      {444, &ref3},
      {666, &ref5},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_val, currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  ref2 = std::move(ref4);

  {

    std::array<TestData, 5> expectedVal = {{
      {111, &ref0},
      {555, &ref2}, // was ref4
      {333, &ref4}, // was ref2
      {444, &ref3},
      {666, &ref5},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_val, currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }

  std::swap(ref2, ref3);

  {

    std::array<TestData, 5> expectedVal = {{
      {111, &ref0},
      {555, &ref3}, // was ref2
      {333, &ref4},
      {444, &ref2}, // was ref3
      {666, &ref5},
    }};

    EXPECT_EQ(bodyManager.size(), expectedVal.size());
    for (std::size_t index = 0; index < expectedVal.size(); ++index) {
      auto& currData = expectedVal.at(index);
      auto& currVal = currData.expectedVal;
      auto& currRef = *currData.pRef;

      EXPECT_EQ(bodyManager.getBody(uint32_t(index))->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_val, currVal)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef.is_active(), true)
        << " => index: " << index << ", value: " << currVal;
      EXPECT_EQ(currRef->_bodyRef->getUserValue(), currVal)
        << " => index: " << index << ", value: " << currVal;
    }
  }
}
