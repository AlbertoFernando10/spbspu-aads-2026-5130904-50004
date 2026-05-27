#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char *argv[]);
#endif

#define BOOST_TEST_MODULE S4
#include <boost/test/included/unit_test.hpp>

#include "bstree.hpp"

#include <string>
using Tree = alberto::BSTree< int, std::string >;
static Tree buildTree()
{
  Tree t;
  t.push(8,  "root");
  t.push(10, "");
  t.push(3,  "hello");
  t.push(1,  "");
  t.push(6,  "");
  t.push(4,  "");
  t.push(14, "");
  t.push(13, "");
  return t;
}
BOOST_AUTO_TEST_CASE(has_existing_key)
{
  const Tree t = buildTree();
  BOOST_TEST(t.has(3) == true);
}

BOOST_AUTO_TEST_CASE(has_missing_key)
{
  const Tree t = buildTree();
  BOOST_TEST(t.has(2) == false);
}
BOOST_AUTO_TEST_CASE(get_existing_key)
{
  const Tree t = buildTree();
  BOOST_TEST(t.get(3) == "hello");
}

BOOST_AUTO_TEST_CASE(get_missing_key_throws)
{
  const Tree t = buildTree();
  BOOST_CHECK_THROW(t.get(99), std::out_of_range);
}
BOOST_AUTO_TEST_CASE(push_new_key)
{
  Tree t = buildTree();
  t.push(12, "twelve");
  BOOST_TEST(t.has(12) == true);
  BOOST_TEST(t.get(12) == "twelve");
}

BOOST_AUTO_TEST_CASE(push_updates_existing_key)
{
  Tree t = buildTree();
  t.push(3, "updated");
  BOOST_TEST(t.get(3) == "updated");
  BOOST_TEST(t.size() == 8u);
}

BOOST_AUTO_TEST_CASE(drop_existing_key)
{
  Tree t = buildTree();
  const std::string val = t.drop(4);
  BOOST_TEST(val == "");
  BOOST_TEST(t.has(4) == false);
  BOOST_TEST(t.size() == 7u);
}

BOOST_AUTO_TEST_CASE(drop_missing_key_throws)
{
  Tree t = buildTree();
  BOOST_CHECK_THROW(t.drop(99), std::out_of_range);
}
BOOST_AUTO_TEST_CASE(size_after_pushes)
{
  const Tree t = buildTree();
  BOOST_TEST(t.size() == 8u);
}

BOOST_AUTO_TEST_CASE(empty_on_new_tree)
{
  const Tree t;
  BOOST_TEST(t.empty() == true);
}

BOOST_AUTO_TEST_CASE(not_empty_after_push)
{
  Tree t;
  t.push(1, "one");
  BOOST_TEST(t.empty() == false);
}
BOOST_AUTO_TEST_CASE(height_of_tree)
{
  const Tree t = buildTree();
  BOOST_TEST(t.height() == 4u);
}

BOOST_AUTO_TEST_CASE(height_of_empty_tree)
{
  const Tree t;
  BOOST_TEST(t.height() == 0u);
}

BOOST_AUTO_TEST_CASE(height_of_subtree_via_iterator)
{
  const Tree t = buildTree();
  const auto it = t.find(1);
  BOOST_REQUIRE(it != t.end());
  BOOST_TEST(t.height(it) == 1u);
}
BOOST_AUTO_TEST_CASE(iterator_begin_is_minimum)
{
  const Tree t = buildTree();
  const auto it = t.begin();
  BOOST_REQUIRE(it != t.end());
  BOOST_TEST(it->first == 1);
}

BOOST_AUTO_TEST_CASE(iterator_inorder_sequence)
{
  const Tree t = buildTree();
  const int expected[] = { 1, 3, 4, 6, 8, 10, 13, 14 };
  int idx = 0;
  for (const auto &kv : t) {
    BOOST_TEST(kv.first == expected[idx]);
    ++idx;
  }
  BOOST_TEST(idx == 8);
}

BOOST_AUTO_TEST_CASE(iterator_decrement_from_end)
{
  const Tree t = buildTree();
  auto it = t.end();
  --it;
  BOOST_TEST(it->first == 14);
}

BOOST_AUTO_TEST_CASE(find_existing_key)
{
  const Tree t = buildTree();
  const auto it = t.find(6);
  BOOST_REQUIRE(it != t.end());
  BOOST_TEST(it->first == 6);
}

BOOST_AUTO_TEST_CASE(find_missing_key_returns_end)
{
  const Tree t = buildTree();
  BOOST_TEST((t.find(99) == t.end()));
}
BOOST_AUTO_TEST_CASE(clear_empties_tree)
{
  Tree t = buildTree();
  t.clear();
  BOOST_TEST(t.empty() == true);
  BOOST_TEST(t.size() == 0u);
  BOOST_TEST((t.begin() == t.end()));
}
BOOST_AUTO_TEST_CASE(copy_constructor)
{
  const Tree original = buildTree();
  const Tree copy     = original;
  BOOST_TEST(copy.size() == original.size());
  BOOST_TEST(copy.has(3) == true);
  BOOST_TEST(copy.get(3) == "hello");
}

BOOST_AUTO_TEST_CASE(copy_is_independent)
{
  Tree original = buildTree();
  Tree copy     = original;
  copy.drop(3);
  BOOST_TEST(original.has(3) == true);
  BOOST_TEST(copy.has(3) == false);
}

BOOST_AUTO_TEST_CASE(move_constructor)
{
  Tree original = buildTree();
  const Tree moved = std::move(original);
  BOOST_TEST(moved.size() == 8u);
  BOOST_TEST(moved.has(3) == true);
  BOOST_TEST(original.empty() == true);
}
BOOST_AUTO_TEST_CASE(rotate_right_preserves_inorder)
{
  Tree t = buildTree();
  const auto it = t.find(3);
  BOOST_REQUIRE(it != t.end());
  t.rotateRight(it);
  const int expected[] = { 1, 3, 4, 6, 8, 10, 13, 14 };
  int idx = 0;
  for (const auto &kv : t) {
    BOOST_TEST(kv.first == expected[idx]);
    ++idx;
  }
}

BOOST_AUTO_TEST_CASE(rotate_left_preserves_inorder)
{
  Tree t = buildTree();
  const auto it = t.find(10);
  BOOST_REQUIRE(it != t.end());
  t.rotateLeft(it);

  const int expected[] = { 1, 3, 4, 6, 8, 10, 13, 14 };
  int idx = 0;
  for (const auto &kv : t) {
    BOOST_TEST(kv.first == expected[idx]);
    ++idx;
  }
}

BOOST_AUTO_TEST_CASE(rotate_right_invalid_throws)
{
  Tree t = buildTree();
  const auto it = t.find(1);
  BOOST_REQUIRE(it != t.end());
  BOOST_CHECK_THROW(t.rotateRight(it), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(rotate_left_invalid_throws)
{
  Tree t = buildTree();
  const auto it = t.find(1);
  BOOST_REQUIRE(it != t.end());
  BOOST_CHECK_THROW(t.rotateLeft(it), std::invalid_argument);
}
