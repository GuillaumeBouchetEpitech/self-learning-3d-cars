
#include "headers.hpp"

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_get_the_values_from_it) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto ref = myPool.acquire(555, "test");

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(ref), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref.is_active(), true);
  EXPECT_EQ(ref, true);
  EXPECT_EQ(ref.get(), myPool.get(0).get());
  EXPECT_EQ(ref->value, 555);
  EXPECT_EQ(ref->my_string, "test");
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_copy_and_move_it) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto mainRef = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef;    // copy
  auto copiedRef2 = copiedRef1; // copy

  auto movedRef = copiedRef2;            // copy
  auto copiedRef3 = std::move(movedRef); // move

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(mainRef), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 4);
  EXPECT_EQ(mainRef.is_active(), true);
  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(movedRef.is_active(), false);
  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(mainRef, true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(movedRef, false);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(mainRef.get(), myPool.get(0).get());
  EXPECT_EQ(movedRef.get(), nullptr);
  EXPECT_EQ(mainRef.get(), copiedRef1.get());
  EXPECT_EQ(mainRef.get(), copiedRef2.get());
  EXPECT_EQ(mainRef.get(), copiedRef3.get());

  EXPECT_EQ(mainRef->value, 555);
  EXPECT_EQ(mainRef->my_string, "test");
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_get_and_set_the_values_of_it) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto ref = myPool.acquire(555, "test");

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(ref), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref.is_active(), true);
  EXPECT_EQ(ref, true);
  EXPECT_EQ(ref.get(), myPool.get(0).get());
  EXPECT_EQ(ref->value, 555);
  EXPECT_EQ(ref->my_string, "test");

  ref->value += 100;
  ref->my_string += " test";

  EXPECT_EQ(ref->value, 655);
  EXPECT_EQ(ref->my_string, "test test");
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_get_total_ref_count_from_the_pool) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto ref = myPool.acquire(555, "test");

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(ref), 0);
  EXPECT_EQ(myPool.get_ref_count(ref), 1);
  EXPECT_EQ(ref.is_active(), true);
  EXPECT_EQ(ref, true);
  EXPECT_EQ(ref.get(), myPool.get(0).get());
  EXPECT_EQ(ref->value, 555);
  EXPECT_EQ(ref->my_string, "test");

  ref.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(ref.is_active(), false);
  EXPECT_EQ(ref, false);
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_weak_ref_and_invalidate_it) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto mainRef = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef;    // copy
  auto copiedRef2 = copiedRef1; // copy
  auto copiedRef3 = copiedRef2; // copy

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(mainRef), 0);
  EXPECT_EQ(myPool.get_index(copiedRef1), 0);
  EXPECT_EQ(myPool.get_index(copiedRef2), 0);
  EXPECT_EQ(myPool.get_index(copiedRef3), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 4);

  EXPECT_EQ(mainRef.is_active(), true);
  EXPECT_EQ(mainRef, true);
  EXPECT_EQ(mainRef.get(), myPool.get(0).get());
  EXPECT_EQ(mainRef->value, 555);
  EXPECT_EQ(mainRef->my_string, "test");

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  mainRef.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 3);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  copiedRef2.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 2);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  copiedRef1.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 1);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);

  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  copiedRef3.invalidate();

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_ref_count(0), 0);

  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);

  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);

  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);

  EXPECT_EQ(copiedRef3.is_active(), false);
  EXPECT_EQ(copiedRef3, false);
}

TEST(weak_ref_data_pool__refs_features, can_acquire_weak_ref_and_release_it) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto mainRef = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef;    // copy
  auto copiedRef2 = copiedRef1; // copy
  auto copiedRef3 = copiedRef2; // copy

  EXPECT_EQ(myPool.size(), 1);
  EXPECT_EQ(myPool.get_index(mainRef), 0);
  EXPECT_EQ(myPool.get_index(copiedRef1), 0);
  EXPECT_EQ(myPool.get_index(copiedRef2), 0);
  EXPECT_EQ(myPool.get_index(copiedRef3), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 4);

  EXPECT_EQ(mainRef.is_active(), true);
  EXPECT_EQ(mainRef, true);
  EXPECT_EQ(mainRef.get(), myPool.get(0).get());
  EXPECT_EQ(mainRef->value, 555);
  EXPECT_EQ(mainRef->my_string, "test");

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef2->value, 555);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef3->value, 555);
  EXPECT_EQ(copiedRef3->my_string, "test");

  myPool.release(mainRef);

  EXPECT_EQ(myPool.size(), 0);
  EXPECT_EQ(mainRef.is_active(), false);
  EXPECT_EQ(mainRef, false);
  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);
  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);
  EXPECT_EQ(copiedRef3.is_active(), false);
  EXPECT_EQ(copiedRef3, false);
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_several_weak_ref_and_release_them_one_by_one) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto ref1 = myPool.acquire(555, "test");
  auto ref2 = myPool.acquire(666, "test");
  auto ref3 = myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);

  EXPECT_EQ(myPool.get_index(ref1), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref1.is_active(), true);
  EXPECT_EQ(ref1, true);
  EXPECT_EQ(ref1.get(), myPool.get(0).get());
  EXPECT_EQ(ref1->value, 555);
  EXPECT_EQ(ref1->my_string, "test");

  EXPECT_EQ(myPool.get_index(ref2), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(ref2.is_active(), true);
  EXPECT_EQ(ref2, true);
  EXPECT_EQ(ref2.get(), myPool.get(1).get());
  EXPECT_EQ(ref2->value, 666);
  EXPECT_EQ(ref2->my_string, "test");

  EXPECT_EQ(myPool.get_index(ref3), 2);
  EXPECT_EQ(myPool.get_ref_count(2), 1);
  EXPECT_EQ(ref3.is_active(), true);
  EXPECT_EQ(ref3, true);
  EXPECT_EQ(ref3.get(), myPool.get(2).get());
  EXPECT_EQ(ref3->value, 777);
  EXPECT_EQ(ref3->my_string, "test");

  myPool.release(ref2);

  EXPECT_EQ(myPool.size(), 2);

  EXPECT_EQ(myPool.get_index(ref1), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref1.is_active(), true);
  EXPECT_EQ(ref1, true);
  EXPECT_EQ(ref1.get(), myPool.get(0).get());
  EXPECT_EQ(ref1->value, 555);
  EXPECT_EQ(ref1->my_string, "test");

  EXPECT_EQ(ref2.is_active(), false);
  EXPECT_EQ(ref2, false);

  EXPECT_EQ(myPool.get_index(ref3), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(ref3.is_active(), true);
  EXPECT_EQ(ref3, true);
  EXPECT_EQ(ref3.get(), myPool.get(1).get());
  EXPECT_EQ(ref3->value, 777);
  EXPECT_EQ(ref3->my_string, "test");

  myPool.release(ref1);

  EXPECT_EQ(myPool.size(), 1);

  EXPECT_EQ(ref1.is_active(), false);
  EXPECT_EQ(ref1, false);

  EXPECT_EQ(ref2.is_active(), false);
  EXPECT_EQ(ref2, false);

  EXPECT_EQ(myPool.get_index(ref3), 0);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(ref3.is_active(), true);
  EXPECT_EQ(ref3, true);
  EXPECT_EQ(ref3.get(), myPool.get(0).get());
  EXPECT_EQ(ref3->value, 777);
  EXPECT_EQ(ref3->my_string, "test");

  myPool.release(ref3);

  EXPECT_EQ(myPool.size(), 0);

  EXPECT_EQ(ref1.is_active(), false);
  EXPECT_EQ(ref1, false);

  EXPECT_EQ(ref2.is_active(), false);
  EXPECT_EQ(ref2, false);

  EXPECT_EQ(ref3.is_active(), false);
  EXPECT_EQ(ref3, false);
}

TEST(weak_ref_data_pool__refs_features,
     can_acquire_several_weak_ref_and_clear_the_pool) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  auto mainRef1 = myPool.acquire(555, "test");
  auto copiedRef1 = mainRef1; // copy

  auto mainRef2 = myPool.acquire(666, "test");
  auto copiedRef2 = mainRef2; // copy

  auto mainRef3 = myPool.acquire(777, "test");
  auto copiedRef3 = mainRef3; // copy

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_index(mainRef1), 0);
  EXPECT_EQ(myPool.get_index(copiedRef1), 0);
  EXPECT_EQ(myPool.get_index(mainRef2), 1);
  EXPECT_EQ(myPool.get_index(copiedRef2), 1);
  EXPECT_EQ(myPool.get_index(mainRef3), 2);
  EXPECT_EQ(myPool.get_index(copiedRef3), 2);
  EXPECT_EQ(myPool.get_ref_count(0), 2);
  EXPECT_EQ(myPool.get_ref_count(1), 2);
  EXPECT_EQ(myPool.get_ref_count(2), 2);

  EXPECT_EQ(mainRef1.is_active(), true);
  EXPECT_EQ(mainRef1, true);
  EXPECT_EQ(mainRef1.get(), myPool.get(0).get());
  EXPECT_EQ(mainRef1->value, 555);
  EXPECT_EQ(mainRef1->my_string, "test");

  EXPECT_EQ(copiedRef1.is_active(), true);
  EXPECT_EQ(copiedRef1, true);
  EXPECT_EQ(copiedRef1.get(), myPool.get(0).get());
  EXPECT_EQ(copiedRef1->value, 555);
  EXPECT_EQ(copiedRef1->my_string, "test");

  EXPECT_EQ(mainRef2.is_active(), true);
  EXPECT_EQ(mainRef2, true);
  EXPECT_EQ(mainRef2.get(), myPool.get(1).get());
  EXPECT_EQ(mainRef2->value, 666);
  EXPECT_EQ(mainRef2->my_string, "test");

  EXPECT_EQ(copiedRef2.is_active(), true);
  EXPECT_EQ(copiedRef2, true);
  EXPECT_EQ(copiedRef2.get(), myPool.get(1).get());
  EXPECT_EQ(copiedRef2->value, 666);
  EXPECT_EQ(copiedRef2->my_string, "test");

  EXPECT_EQ(mainRef3.is_active(), true);
  EXPECT_EQ(mainRef3, true);
  EXPECT_EQ(mainRef3.get(), myPool.get(2).get());
  EXPECT_EQ(mainRef3->value, 777);
  EXPECT_EQ(mainRef3->my_string, "test");

  EXPECT_EQ(copiedRef3.is_active(), true);
  EXPECT_EQ(copiedRef3, true);
  EXPECT_EQ(copiedRef3.get(), myPool.get(2).get());
  EXPECT_EQ(copiedRef3->value, 777);
  EXPECT_EQ(copiedRef3->my_string, "test");

  myPool.clear();

  EXPECT_EQ(myPool.size(), 0);
  EXPECT_EQ(mainRef1.is_active(), false);
  EXPECT_EQ(mainRef1, false);
  EXPECT_EQ(copiedRef1.is_active(), false);
  EXPECT_EQ(copiedRef1, false);
  EXPECT_EQ(mainRef2.is_active(), false);
  EXPECT_EQ(mainRef2, false);
  EXPECT_EQ(copiedRef2.is_active(), false);
  EXPECT_EQ(copiedRef2, false);
  EXPECT_EQ(mainRef3.is_active(), false);
  EXPECT_EQ(mainRef3, false);
  EXPECT_EQ(copiedRef3.is_active(), false);
  EXPECT_EQ(copiedRef3, false);
}

TEST(weak_ref_data_pool__refs_features, can_directly_access_pool_data) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  myPool.acquire(555, "test");
  myPool.acquire(666, "test");
  myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(myPool.get_ref_count(1), 0);
  EXPECT_EQ(myPool.get_ref_count(2), 0);

  EXPECT_EQ(myPool.get(0)->value, 555);
  EXPECT_EQ(myPool.get(0)->my_string, "test");
  EXPECT_EQ(myPool.get(1)->value, 666);
  EXPECT_EQ(myPool.get(1)->my_string, "test");
  EXPECT_EQ(myPool.get(2)->value, 777);
  EXPECT_EQ(myPool.get(2)->my_string, "test");
}

TEST(weak_ref_data_pool__refs_features, can_prevent_pool_growth) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  for (int ii = 0; ii < 20; ++ii) {
    auto ref = myPool.acquire(ii);
    if (ii < 10) {
      EXPECT_EQ(ref.is_active(), true);
    } else {
      EXPECT_EQ(ref.is_active(), false);
    }
  }

  EXPECT_EQ(myPool.size(), 10);
  for (int ii = 0; ii < 10; ++ii)
    EXPECT_EQ(myPool.get(ii).get()->value, ii);
}

TEST(weak_ref_data_pool__refs_features, can_allow_pool_growth) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            false>
    myPool;

  for (int ii = 0; ii < 20; ++ii) {
    auto ref = myPool.acquire(ii);
    EXPECT_EQ(ref.is_active(), true);
  }

  EXPECT_EQ(myPool.size(), 20);
  for (int ii = 0; ii < 20; ++ii)
    EXPECT_EQ(myPool.get(ii).get()->value, ii);
}

TEST(weak_ref_data_pool__refs_features, can_move_data_into_a_weak_ref) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            false>
    myPool;

  auto mainRef = myPool.acquire(666, "test");

  EXPECT_EQ(mainRef->value, 666);
  EXPECT_EQ(mainRef->my_string, "test");

  common::TestStructure tmpData(777, "test test");
  *mainRef = std::move(tmpData);

  EXPECT_EQ(mainRef->value, 777);
  EXPECT_EQ(mainRef->my_string, "test test");
}

TEST(weak_ref_data_pool__refs_features, can_move_entire_pool) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            false>
    myPool1;
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            false>
    myPool2;

  for (int ii = 0; ii < 20; ++ii)
    myPool1.acquire(ii, "test");

  EXPECT_EQ(myPool1.size(), 20);
  EXPECT_EQ(myPool2.size(), 0);
  for (int ii = 0; ii < 20; ++ii) {
    EXPECT_EQ(myPool1.get(ii)->value, ii);
    EXPECT_EQ(myPool1.get(ii)->my_string, "test");
  }

  myPool2 = std::move(myPool1);

  EXPECT_EQ(myPool1.size(), 0);
  EXPECT_EQ(myPool2.size(), 20);
  for (int ii = 0; ii < 20; ++ii) {
    EXPECT_EQ(myPool2.get(ii)->value, ii);
    EXPECT_EQ(myPool2.get(ii)->my_string, "test");
  }
}

TEST(weak_ref_data_pool__refs_features,
     can_make_weak_ref_that_invalidate_once_out_of_scope) {
  gero::weak_ref_data_pool<common::TestStructure, common::TestStructure, 10,
                            true>
    myPool;

  myPool.acquire(555, "test");
  myPool.acquire(666, "test");
  myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(myPool.get_ref_count(1), 0);
  EXPECT_EQ(myPool.get_ref_count(2), 0);

  {
    auto ref1 = myPool.get(0);
    auto ref2 = myPool.get(1);
    auto ref3 = myPool.get(2);

    EXPECT_EQ(myPool.get_ref_count(0), 1);
    EXPECT_EQ(myPool.get_ref_count(1), 1);
    EXPECT_EQ(myPool.get_ref_count(2), 1);
  }

  EXPECT_EQ(myPool.get_ref_count(0), 0);
  EXPECT_EQ(myPool.get_ref_count(1), 0);
  EXPECT_EQ(myPool.get_ref_count(2), 0);
}

// TEST(weak_ref_data_pool__refs_features,
// can_make_weak_ref_that_invalidate_once_out_of_scope_with_get_index_feature) {

//   using LocalPool = gero::weak_ref_data_pool<common::TestStructure,
//   common::TestStructure, 10, true>; using LocalRef = LocalPool::weak_ref;

//   LocalPool myPool;

//   LocalRef mainRef1 = myPool.acquire(555, "test");
//   LocalRef mainRef2 = myPool.acquire(666, "test");
//   LocalRef mainRef3 = myPool.acquire(777, "test");

//   EXPECT_EQ(myPool.size(), 3);
//   EXPECT_EQ(myPool.get_ref_count(0), 1);
//   EXPECT_EQ(myPool.get_ref_count(1), 1);
//   EXPECT_EQ(myPool.get_ref_count(2), 1);

//   {
//     LocalRef subRef1 = myPool.get(0);
//     LocalRef subRef2 = myPool.get(1);
//     LocalRef subRef3 = myPool.get(2);

//     EXPECT_EQ(myPool.get_ref_count(0), 2);
//     EXPECT_EQ(myPool.get_ref_count(1), 2);
//     EXPECT_EQ(myPool.get_ref_count(2), 2);
//   }

//   EXPECT_EQ(myPool.get_ref_count(0), 1);
//   EXPECT_EQ(myPool.get_ref_count(1), 1);
//   EXPECT_EQ(myPool.get_ref_count(2), 1);
// }

namespace {

using LocalPool = gero::weak_ref_data_pool<common::TestStructure,
                                            common::TestStructure, 10, true>;
using LocalRef = LocalPool::weak_ref;
using LocalValue = LocalPool::value_type;

struct TmpResult {
  int val1;
  int val2;
  LocalRef ref;
};

struct ResultRaw {
  bool hasHit = false;

  TmpResult* allRawImpactsData;
  std::size_t allImpactsMaxSize;
  std::size_t allImpactsTotal;
};

template <std::size_t N> struct ResultArray {
  bool hasHit = false;

  std::array<TmpResult, N> allImpactsData;
  std::size_t allImpactsTotal;
};
} // namespace

TEST(
  weak_ref_data_pool__refs_features,
  can_make_weak_ref_that_invalidate_once_out_of_scope_with_get_index_feature_2) {

  LocalPool myPool;

  LocalRef mainRef1 = myPool.acquire(555, "test");
  LocalRef mainRef2 = myPool.acquire(666, "test");
  LocalRef mainRef3 = myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(myPool.get_ref_count(2), 1);

  {

    constexpr std::size_t k_size = 10;

    ResultArray<k_size> tmpArray;
    ResultRaw tmpRaw;
    tmpRaw.allRawImpactsData = tmpArray.allImpactsData.data();
    tmpRaw.allImpactsMaxSize = k_size;
    tmpRaw.allImpactsTotal = 0;

    tmpRaw.allRawImpactsData[tmpRaw.allImpactsTotal++].ref = myPool.get(0);
    tmpRaw.allRawImpactsData[tmpRaw.allImpactsTotal++].ref = myPool.get(1);
    LocalRef subRef3 = myPool.get(2);

    EXPECT_EQ(tmpRaw.allImpactsTotal, 2);

    EXPECT_EQ(myPool.get_ref_count(0), 2);
    EXPECT_EQ(myPool.get_ref_count(1), 2);
    EXPECT_EQ(myPool.get_ref_count(2), 2);
  }

  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(myPool.get_ref_count(2), 1);
}

TEST(
  weak_ref_data_pool__refs_features,
  can_make_weak_ref_that_invalidate_once_out_of_scope_with_get_index_feature_3) {

  LocalPool myPool;

  LocalRef mainRef1 = myPool.acquire(555, "test");

  // {
  //   const LocalValue& val = *mainRef1;
  //   D_MYERR("val addr  " << reinterpret_cast<int64_t>(&val));
  //   LocalRef tmpRef = myPool.get(val);
  //   D_MYERR("tmpRef._pool  " << tmpRef._pool);
  //   D_MYERR("tmpRef._index " << tmpRef._index);
  // }

  LocalRef mainRef2 = myPool.acquire(666, "test");
  LocalRef mainRef3 = myPool.acquire(777, "test");

  EXPECT_EQ(myPool.size(), 3);
  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(myPool.get_ref_count(2), 1);

  {

    constexpr std::size_t k_size = 10;

    ResultArray<k_size> tmpArray;
    ResultRaw tmpRaw;
    tmpRaw.allRawImpactsData = tmpArray.allImpactsData.data();
    tmpRaw.allImpactsMaxSize = k_size;
    tmpRaw.allImpactsTotal = 0;

    // LocalRef testRef = LocalRef::make_invalid();
    LocalRef testRef;

    D_MYERR("testRef._pool  " << testRef._pool);
    D_MYERR("testRef._index " << testRef._index);

    EXPECT_EQ(testRef, LocalRef::make_invalid());

#if 1

    LocalRef found = myPool.find_if([&mainRef2](const LocalValue& target) {
      return &target == mainRef2.get();
    });

    D_MYERR("found._pool  " << found._pool);
    D_MYERR("found._index " << found._index);

    testRef = found;

#else

    // testRef = myPool.get(*mainRef2.get());
    testRef = myPool.get(*mainRef2);

#endif

    D_MYERR("testRef._pool  " << testRef._pool);
    D_MYERR("testRef._index " << testRef._index);

    EXPECT_EQ(testRef, mainRef2);
    // EXPECT_EQ(testRef, myPool.get(*mainRef1));
  }

  // {
  //   EXPECT_EQ(tmpRaw.allRawImpactsData[0].ref, LocalRef::make_invalid());
  //   EXPECT_EQ(tmpRaw.allRawImpactsData[1].ref, LocalRef::make_invalid());
  //   EXPECT_EQ(tmpRaw.allRawImpactsData[2].ref, LocalRef::make_invalid());

  //   tmpRaw.allRawImpactsData[tmpRaw.allImpactsTotal++].ref =
  //   myPool.get(*mainRef1);
  //   tmpRaw.allRawImpactsData[tmpRaw.allImpactsTotal++].ref =
  //   myPool.get(*mainRef2);
  //   tmpRaw.allRawImpactsData[tmpRaw.allImpactsTotal++].ref =
  //   myPool.get(*mainRef3);
  //   // LocalRef subRef3 = myPool.get(2);

  //   EXPECT_EQ(tmpRaw.allImpactsTotal, 3);
  //   EXPECT_EQ(tmpRaw.allRawImpactsData[0].ref, mainRef1);
  //   EXPECT_EQ(tmpRaw.allRawImpactsData[1].ref, mainRef2);
  //   EXPECT_EQ(tmpRaw.allRawImpactsData[2].ref, mainRef3);

  //   EXPECT_EQ(myPool.get_ref_count(0), 2);
  //   EXPECT_EQ(myPool.get_ref_count(1), 2);
  //   EXPECT_EQ(myPool.get_ref_count(2), 2);
  // }

  EXPECT_EQ(myPool.get_ref_count(0), 1);
  EXPECT_EQ(myPool.get_ref_count(1), 1);
  EXPECT_EQ(myPool.get_ref_count(2), 1);
}
