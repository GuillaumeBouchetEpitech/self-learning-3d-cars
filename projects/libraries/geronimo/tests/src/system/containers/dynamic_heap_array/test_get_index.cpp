
// #include "headers.hpp"

// #include "system/containers/generic_array_container_commons/common.hpp"

// TEST(dynamic_heap_array, get_index) {

//   dynamic_heap_array<common::TestStructure> myDefaultDynamicArray;
//   EXPECT_EQ(myDefaultDynamicArray.size(), 0);
//   EXPECT_EQ(myDefaultDynamicArray.capacity(), 0);
//   EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

//   myDefaultDynamicArray.pre_allocate(10);

//   EXPECT_EQ(myDefaultDynamicArray.size(), 0);
//   EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
//   EXPECT_EQ(myDefaultDynamicArray.is_empty(), true);

//   for (int ii = 0; ii < 10; ++ii)
//   {
//     myDefaultDynamicArray.emplace_back(ii);
//   }

//   EXPECT_EQ(myDefaultDynamicArray.size(), 10);
//   EXPECT_EQ(myDefaultDynamicArray.capacity(), 10);
//   EXPECT_EQ(myDefaultDynamicArray.is_empty(), false);

//   for (int ii = 0; ii < 10; ++ii)
//   {
//     EXPECT_EQ(myDefaultDynamicArray.at(ii).value, ii);
//     EXPECT_EQ(myDefaultDynamicArray.get_index(myDefaultDynamicArray.at(ii)),
//     ii);
//   }

//   const common::TestStructure* fakedData = (common::TestStructure*)(666);
//   EXPECT_EQ(myDefaultDynamicArray.get_index(*fakedData), -1);
// }
