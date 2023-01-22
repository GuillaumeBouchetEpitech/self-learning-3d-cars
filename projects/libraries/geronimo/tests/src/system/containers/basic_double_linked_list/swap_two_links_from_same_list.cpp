
//

#include "headers.hpp"

TEST(basic_double_linked_list__swap_two_links_from_same_list,
     can_swap_link_in_2_links_list) {
  List list;
  Link link1;
  Link link2;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link1, link2);

  EXPECT_EQ(list.head_link, &link1);
  EXPECT_EQ(list.size, 2);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, &link2);
  EXPECT_EQ(link2.prev_link, &link1);
  EXPECT_EQ(link2.next_link, nullptr);
}

TEST(basic_double_linked_list__swap_two_links_from_same_list,
     can_swap_link_in_3_links_list) {
  List list;
  Link link1;
  Link link2;
  Link link3;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);
  List::add_link_to_list(list, link3);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link2);
  EXPECT_EQ(link2.prev_link, &link3);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link1, link2);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link3);
  EXPECT_EQ(link1.next_link, &link2);
  EXPECT_EQ(link2.prev_link, &link1);
  EXPECT_EQ(link2.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link3, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, &link3);
  EXPECT_EQ(link3.prev_link, &link1);
  EXPECT_EQ(link3.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link1, link3);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link3);
  EXPECT_EQ(link3.prev_link, &link2);
  EXPECT_EQ(link3.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link3);
  EXPECT_EQ(link1.next_link, nullptr);
}

TEST(basic_double_linked_list__swap_two_links_from_same_list,
     can_swap_dead_link_in_3_links_list) {
  List list;
  Link link1;
  Link link2;
  Link link3;
  Link link4;

  EXPECT_EQ(list.head_link, nullptr);
  EXPECT_EQ(list.size, 0);
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, nullptr);
  EXPECT_EQ(link4.prev_link, nullptr);
  EXPECT_EQ(link4.next_link, nullptr);

  List::add_link_to_list(list, link1);
  List::add_link_to_list(list, link2);
  List::add_link_to_list(list, link3);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link2);
  EXPECT_EQ(link2.prev_link, &link3);
  EXPECT_EQ(link2.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link2);
  EXPECT_EQ(link1.next_link, nullptr);
  //
  EXPECT_EQ(link4.prev_link, nullptr);
  EXPECT_EQ(link4.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link2, link4);

  EXPECT_EQ(list.head_link, &link3);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, &link4);
  EXPECT_EQ(link4.prev_link, &link3);
  EXPECT_EQ(link4.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link4);
  EXPECT_EQ(link1.next_link, nullptr);
  //
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link3, link2);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link4);
  EXPECT_EQ(link4.prev_link, &link2);
  EXPECT_EQ(link4.next_link, &link1);
  EXPECT_EQ(link1.prev_link, &link4);
  EXPECT_EQ(link1.next_link, nullptr);
  //
  EXPECT_EQ(link3.prev_link, nullptr);
  EXPECT_EQ(link3.next_link, nullptr);

  List::swap_two_links_from_same_list(list, link1, link3);

  EXPECT_EQ(list.head_link, &link2);
  EXPECT_EQ(list.size, 3);
  EXPECT_EQ(link2.prev_link, nullptr);
  EXPECT_EQ(link2.next_link, &link4);
  EXPECT_EQ(link4.prev_link, &link2);
  EXPECT_EQ(link4.next_link, &link3);
  EXPECT_EQ(link3.prev_link, &link4);
  EXPECT_EQ(link3.next_link, nullptr);
  //
  EXPECT_EQ(link1.prev_link, nullptr);
  EXPECT_EQ(link1.next_link, nullptr);
}
