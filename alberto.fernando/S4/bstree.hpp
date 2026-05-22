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
  explicit BSTree(const Compare &cmp = Compare()):
    fakeLeaf_(new Node(Node::Kind::FAKE_LEAF)),
    fakeRoot_(new Node(Node::Kind::FAKE_ROOT)),
    size_(0),
    cmp_(cmp)
  {
    fakeLeaf_->left_   = fakeLeaf_;
    fakeLeaf_->right_  = fakeLeaf_;
    fakeLeaf_->parent_ = fakeRoot_;
    fakeRoot_->left_   = fakeLeaf_;
    fakeRoot_->right_  = fakeLeaf_;
    fakeRoot_->parent_ = nullptr;
  }

  BSTree(const BSTree &o):
    BSTree(o.cmp_)
  {
    if (o.root()->isReal()) {
      rootRef() = cloneTree(o.root(), fakeRoot_);
      size_     = o.size_;
    }
  }

  BSTree(BSTree &&o) noexcept:
    fakeLeaf_(o.fakeLeaf_),
    fakeRoot_(o.fakeRoot_),
    size_(o.size_),
    cmp_(std::move(o.cmp_))
  {
    o.fakeLeaf_ = nullptr;
    o.fakeRoot_ = nullptr;
    o.size_     = 0;
  }

  BSTree &operator=(BSTree o) noexcept
  {
    swap(o);
    return *this;
  }

  ~BSTree()
  {
    if (fakeRoot_) {
      destroy(rootRef());
      delete fakeRoot_;
    }
    delete fakeLeaf_;
  }

  void swap(BSTree &o) noexcept
  {
    std::swap(fakeLeaf_, o.fakeLeaf_);
    std::swap(fakeRoot_, o.fakeRoot_);
    std::swap(size_,     o.size_);
    std::swap(cmp_,      o.cmp_);
  }

  bool empty() const noexcept
  {
    return size_ == 0;
  }

  size_type size() const noexcept
  {
    return size_;
  }
  iterator begin() noexcept
  {
    if (rootRef()->isFake()) {
      return end();
    }
    return iterator(leftmost(rootRef()));
  }

  iterator end() noexcept
  {
    return iterator(fakeRoot_);
  }

  const_iterator begin() const noexcept
  {
    if (root()->isFake()) {
      return end();
    }
    return const_iterator(leftmost(const_cast< Node * >(root())));
  }

  const_iterator end() const noexcept
  {
    return const_iterator(fakeRoot_);
  }

  const_iterator cbegin() const noexcept
  {
    return begin();
  }

  const_iterator cend() const noexcept
  {
    return end();
  }

  iterator find(const Key &k) noexcept
  {
    Node *cur = rootRef();
    while (cur->isReal()) {
      if (cmp_(k, cur->key())) {
        cur = cur->left_;
      } else if (cmp_(cur->key(), k)) {
        cur = cur->right_;
      } else {
        return iterator(cur);
      }
    }
    return end();
  }

  const_iterator find(const Key &k) const noexcept
  {
    const Node *cur = root();
    while (cur->isReal()) {
      if (cmp_(k, cur->key())) {
        cur = cur->left_;
      } else if (cmp_(cur->key(), k)) {
        cur = cur->right_;
      } else {
        return const_iterator(cur);
      }
    }
    return end();
  }

  bool has(const Key &k) const noexcept
  {
    return find(k) != end();
  }
