#ifndef BSTREE_HPP
#define BSTREE_HPP

#include "bstnode.hpp"
#include "bstiterator.hpp"

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
  iterator push(const Key &k, const Value &v)
  {
    Node *parent = fakeRoot_;
    Node *cur    = rootRef();
    bool goLeft  = true;

    while (cur->isReal()) {
      parent = cur;
      if (cmp_(k, cur->key())) {
        cur    = cur->left_;
        goLeft = true;
      } else if (cmp_(cur->key(), k)) {
        cur    = cur->right_;
        goLeft = false;
      } else {
        cur->value() = v;
        return iterator(cur);
      }
    }

    Node *n    = new Node(k, v, fakeLeaf_);
    n->parent_ = parent;

    if (parent->isFakeRoot()) {
      fakeRoot_->left_ = n;
    } else if (goLeft) {
      parent->left_ = n;
    } else {
      parent->right_ = n;
    }

    ++size_;
    return iterator(n);
  }

  Value &get(const Key &k)
  {
    auto it = find(k);
    if (it == end()) {
      throw std::out_of_range("BSTree::get — key not found");
    }
    return it->second;
  }

  const Value &get(const Key &k) const
  {
    auto it = find(k);
    if (it == end()) {
      throw std::out_of_range("BSTree::get — key not found");
    }
    return it->second;
  }

  Value &operator[](const Key &k)
  {
    auto it = find(k);
    if (it != end()) {
      return it->second;
    }
    return push(k, Value{})->second;
  }

  Value drop(const Key &k)
  {
    auto it = find(k);
    if (it == end()) {
      throw std::out_of_range("BSTree::drop — key not found");
    }
    Value v = std::move(it->second);
    erase(it);
    return v;
  }

  iterator erase(iterator it)
  {
    Node *z = it.node_;
    if (!z->isReal()) {
      throw std::invalid_argument("BSTree::erase — invalid iterator");
    }

    iterator succ = std::next(it);

    if (!z->left_->isReal()) {
      transplant(z, z->right_);
    } else if (!z->right_->isReal()) {
      transplant(z, z->left_);
    } else {
      Node *y = leftmost(z->right_);
      if (y->parent_ != z) {
        transplant(y, y->right_);
        y->right_          = z->right_;
        y->right_->parent_ = y;
      }
      transplant(z, y);
      y->left_          = z->left_;
      y->left_->parent_ = y;
    }

    --size_;
    delete z;
    return succ;
  }

  void clear() noexcept
  {
    destroy(rootRef());
    rootRef() = fakeLeaf_;
    size_     = 0;
  }
  const_iterator rotateRight(const_iterator it)
  {
    Node *p = const_cast< Node * >(it.node_);
    if (!p->isReal()) {
      throw std::invalid_argument("rotateRight: invalid node");
    }
    Node *x = p->left_;
    if (!x->isReal()) {
      throw std::invalid_argument("rotateRight: no left child");
    }

    p->left_ = x->right_;
    if (x->right_->isReal()) {
      x->right_->parent_ = p;
    }

    x->parent_ = p->parent_;
    transplant(p, x);
    x->right_  = p;
    p->parent_ = x;

    return const_iterator(x);
  }

  const_iterator rotateLeft(const_iterator it)
  {
    Node *p = const_cast< Node * >(it.node_);
    if (!p->isReal()) {
      throw std::invalid_argument("rotateLeft: invalid node");
    }
    Node *x = p->right_;
    if (!x->isReal()) {
      throw std::invalid_argument("rotateLeft: no right child");
    }

    p->right_ = x->left_;
    if (x->left_->isReal()) {
      x->left_->parent_ = p;
    }

    x->parent_ = p->parent_;
    transplant(p, x);
    x->left_   = p;
    p->parent_ = x;

    return const_iterator(x);
  }

  const_iterator rotateLargeLeft(const_iterator it)
  {
    Node *p = const_cast< Node * >(it.node_);
    if (!p->isReal()) {
      throw std::invalid_argument("rotateLargeLeft: invalid node");
    }
    rotateRight(const_iterator(p->right_));
    return rotateLeft(const_iterator(p));
  }

  const_iterator rotateLargeRight(const_iterator it)
  {
    Node *p = const_cast< Node * >(it.node_);
    if (!p->isReal()) {
      throw std::invalid_argument("rotateLargeRight: invalid node");
    }
    rotateLeft(const_iterator(p->left_));
    return rotateRight(const_iterator(p));
  }

  size_type height(const_iterator it) const noexcept
  {
    const Node *n = it.node_;
    if (!n->isReal()) {
      return 0;
    }
    const size_type lh = height(const_iterator(n->left_));
    const size_type rh = height(const_iterator(n->right_));
    return 1 + (lh > rh ? lh : rh);
  }

  size_type height() const noexcept
  {
    if (cbegin() == cend()) {
      return 0;
    }
    return height(const_iterator(root()));
  }

private:
  using Node = BSTNode< Key, Value >;

  Node     *fakeLeaf_;
  Node     *fakeRoot_;
  size_type size_;
  Compare   cmp_;

  Node *&rootRef() noexcept
  {
    return fakeRoot_->left_;
  }

  const Node *root() const noexcept
  {
    return fakeRoot_->left_;
  }

  Node *leftmost(Node *n) const noexcept
  {
    while (n->left_->isReal()) {
      n = n->left_;
    }
    return n;
  }

  Node *rightmost(Node *n) const noexcept
  {
    while (n->right_->isReal()) {
      n = n->right_;
    }
    return n;
  }

  void transplant(Node *oldNode, Node *n) noexcept
  {
    Node *p = oldNode->parent_;
    if (p->left_ == oldNode) {
      p->left_ = n;
    } else {
      p->right_ = n;
    }
    if (n->isReal()) {
      n->parent_ = p;
    }
  }

  void destroy(Node *n) noexcept
  {
    if (!n || n->isFake()) {
      return;
    }
    destroy(n->left_);
    destroy(n->right_);
    delete n;
  }

  Node *cloneTree(const Node *src, Node *parent)
  {
    if (src->isFake()) {
      return fakeLeaf_;
    }
    Node *n    = new Node(src->key(), src->value(), fakeLeaf_);
    n->parent_ = parent;
    n->left_   = cloneTree(src->left_,  n);
    n->right_  = cloneTree(src->right_, n);
    return n;
  }
};

}

#endif
