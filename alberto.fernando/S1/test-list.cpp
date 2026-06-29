#define BOOST_TEST_MODULE ListTests
#include <boost/test/included/unit_test.hpp>
#include "List.h"

using namespace alberto;
BOOST_AUTO_TEST_CASE(test_empty_on_default_construction) {
  List<int> list;
  BOOST_TEST(list.empty());
  BOOST_TEST(list.size() == 0);
}
BOOST_AUTO_TEST_CASE(test_push_back_increases_size) {
  List<int> list;
  list.push_back(42);
  BOOST_TEST(list.size() == 1);
  BOOST_TEST(list.back() == 42);
}

BOOST_AUTO_TEST_CASE(test_push_front_increases_size) {
  List<int> list;
  list.push_front(17);
  BOOST_TEST(list.size() == 1);
  BOOST_TEST(list.front() == 17);
}
BOOST_AUTO_TEST_CASE(test_front_and_back_after_multiple_pushes) {
  List<int> list;
  list.push_back(42);
  list.push_front(17);
  list.push_back(99);
  BOOST_TEST(list.front() == 17);
  BOOST_TEST(list.back() == 99);
}
BOOST_AUTO_TEST_CASE(test_index_operator) {
  List<int> list;
  list.push_back(42);
  list.push_front(17);
  list.push_back(99);
  BOOST_TEST(list[0] == 17);
  BOOST_TEST(list[1] == 42);
  BOOST_TEST(list[2] == 99);
}
BOOST_AUTO_TEST_CASE(test_pop_front_removes_first_element) {
  List<int> list;
  list.push_back(42);
  list.push_front(17);
  list.push_back(99);
  list.pop_front();
  BOOST_TEST(list.size() == 2);
  BOOST_TEST(list.front() == 42);
}

BOOST_AUTO_TEST_CASE(test_pop_back_removes_last_element) {
  List<int> list;
  list.push_back(42);
  list.push_front(17);
  list.push_back(99);
  list.pop_back();
  BOOST_TEST(list.size() == 2);
  BOOST_TEST(list.back() == 42);
}
BOOST_AUTO_TEST_CASE(test_clear_empties_list) {
  List<int> list;
  list.push_back(42);
  list.push_front(17);
  list.clear();
  BOOST_TEST(list.empty());
  BOOST_TEST(list.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_forward_iteration) {
  List<int> list;
  for (int i = 100; i <= 300; i += 100)
    list.push_back(i);

  auto it = list.begin();
  BOOST_TEST(*it == 100);
  ++it; BOOST_TEST(*it == 200);
  ++it; BOOST_TEST(*it == 300);
}

BOOST_AUTO_TEST_CASE(test_backward_iteration) {
  List<int> list;
  for (int i = 100; i <= 300; i += 100)
    list.push_back(i);

  auto it = list.begin();
  ++it; ++it;
  --it; BOOST_TEST(*it == 200);
  --it; BOOST_TEST(*it == 100);
}
BOOST_AUTO_TEST_CASE(test_copy_constructor_produces_independent_copy) {
  List<int> list1;
  list1.push_back(10);
  list1.push_back(20);
  list1.push_back(30);

  List<int> list2(list1);
  BOOST_TEST(list2.size() == 3);
  BOOST_TEST(list2[0] == 10);
  BOOST_TEST(list2[1] == 20);
  BOOST_TEST(list2[2] == 30);
}
BOOST_AUTO_TEST_CASE(test_erase_middle_element) {
  List<int> list;
  list.push_back(10);
  list.push_back(20);
  list.push_back(30);

  auto it = list.begin();
  ++it;
  list.erase(it);

  BOOST_TEST(list.size() == 2);
  BOOST_TEST(list[0] == 10);
  BOOST_TEST(list[1] == 30);
}
