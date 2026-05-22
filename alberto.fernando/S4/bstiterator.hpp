#ifndef BST_ITERATOR_HPP
#define BST_ITERATOR_HPP

#include "bst_node.hpp"

#include <iterator>
#include <cstddef>
namespace alberto {

template< class Key, class Value, class Compare > class BSTree;
template< class Key, class Value > class BSTConstIterator;

template< class Key, class Value >
class BSTIterator {
public:
  using value_type        = std::pair< const Key, Value >;
  using reference         = value_type &;
  using pointer           = value_type *;
  using difference_type   = std::ptrdiff_t;
  using iterator_category = std::bidirectional_iterator_tag;

  BSTIterator() noexcept:
    node_(nullptr)
  {}

  reference operator*() const
  {
    return node_->pair();
  }

  pointer operator->() const
  {
    return node_->data_;
  }
