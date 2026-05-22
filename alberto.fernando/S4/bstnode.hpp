#ifndef BST_NODE_HPP
#define BST_NODE_HPP

#include <utility>
namespace alberto {

template< class Key, class Value >
struct BSTNode {
  using pair_t = std::pair< const Key, Value >;

  enum class Kind { FAKE_ROOT, FAKE_LEAF, REAL };

  Kind     kind_;
  BSTNode *parent_;
  BSTNode *left_;
  BSTNode *right_;
  pair_t  *data_;
