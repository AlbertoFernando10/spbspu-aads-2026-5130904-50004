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
  BSTIterator &operator++() noexcept
  {
    if (node_->right_->isReal()) {
      node_ = node_->right_;
      while (node_->left_->isReal()) {
        node_ = node_->left_;
      }
    } else {
      BSTNode< Key, Value > *p = node_->parent_;
      while (p->isReal() && node_ == p->right_) {
        node_ = p;
        p = p->parent_;
      }
      node_ = p;
    }
    return *this;
  }

  BSTIterator operator++(int) noexcept
  {
    auto t = *this;
    ++(*this);
    return t;
  }

  BSTIterator &operator--() noexcept
  {
    if (node_->left_->isReal()) {
      node_ = node_->left_;
      while (node_->right_->isReal()) {
        node_ = node_->right_;
      }
    } else {
      BSTNode< Key, Value > *p = node_->parent_;
      while (p->isReal() && node_ == p->left_) {
        node_ = p;
        p = p->parent_;
      }
      node_ = p;
    }
    return *this;
  }

  BSTIterator operator--(int) noexcept
  {
    auto t = *this;
    --(*this);
    return t;
  }
  bool operator==(const BSTIterator &o) const noexcept
  {
    return node_ == o.node_;
  }

  bool operator!=(const BSTIterator &o) const noexcept
  {
    return node_ != o.node_;
  }

private:
  using Node = BSTNode< Key, Value >;

  Node *node_;

  explicit BSTIterator(Node *n) noexcept:
    node_(n)
  {}

  template< class K, class V, class C > friend class BSTree;
  friend class BSTConstIterator< Key, Value >;
};
