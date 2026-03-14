#define BOOST_TEST_MODULE ListTests
#include <boost/test/unit_test.hpp>
#include "List.h"

using namespace alberto;

BOOST_AUTO_TEST_CASE(test_basic_operations) {
  List<int> list;
  BOOST_CHECK(list.empty());
  BOOST_CHECK_EQUAL(list.size(), 0);
  list.push_back(42);
  list.push_front(17);
  list.push_back(99);
  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list.front(), 17);
  BOOST_CHECK_EQUAL(list.back(), 99);
  BOOST_CHECK_EQUAL(list[0], 17);
  BOOST_CHECK_EQUAL(list[1], 42);
  BOOST_CHECK_EQUAL(list[2], 99);
  list.pop_front();
  list.pop_back();
  BOOST_CHECK_EQUAL(list.size(), 1);
  BOOST_CHECK_EQUAL(list.front(), 42);
  list.clear();
  BOOST_CHECK(list.empty());
}

BOOST_AUTO_TEST_CASE(test_bidirectional_iterators) {
  List<int> list;
  for (int i = 100; i <= 300; i += 100) {
    list.push_back(i);
  }
  auto it = list.begin();
  BOOST_CHECK_EQUAL(*it, 100);
  ++it; BOOST_CHECK_EQUAL(*it, 200);
  ++it; BOOST_CHECK_EQUAL(*it, 300);
  --it; BOOST_CHECK_EQUAL(*it, 200);
  --it; BOOST_CHECK_EQUAL(*it, 100);
}

BOOST_AUTO_TEST_CASE(test_copy_and_erase) {
  List<int> list1;
  list1.push_back(10);
  list1.push_back(20);
  list1.push_back(30);
  List<int> list2(list1);
  BOOST_CHECK_EQUAL(list2.size(), 3);
  auto it = list2.begin();
  ++it;
  list2.erase(it);
  BOOST_CHECK_EQUAL(list2.size(), 2);
  BOOST_CHECK_EQUAL(list2[0], 10);
  BOOST_CHECK_EQUAL(list2[1], 30);
}
