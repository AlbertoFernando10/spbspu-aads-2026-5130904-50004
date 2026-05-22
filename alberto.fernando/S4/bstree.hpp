#ifndef BSTREE_HPP
#define BSTREE_HPP

#include "bst_node.hpp"
#include "bst_iterator.hpp"

#include <functional>
#include <stdexcept>
#include <utility>

namespace alberto {

template< class Key, class Value, class Compare = std::less< Key > >
class BSTree {
public:
  using key_type       = Key;
  using mapped_type    = Value;
  using value_type     = std::pair< const Key, Value >;
  using size_type      = std::size_t;
  using iterator       = BSTIterator< Key, Value >;
  using const_iterator = BSTConstIterator< Key, Value >;
