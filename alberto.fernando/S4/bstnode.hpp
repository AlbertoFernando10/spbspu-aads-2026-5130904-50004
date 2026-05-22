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
  explicit BSTNode(Kind k) noexcept:
    kind_(k),
    parent_(nullptr),
    left_(nullptr),
    right_(nullptr),
    data_(nullptr)
  {}

  BSTNode(const Key &k, const Value &v, BSTNode *fakeLeaf):
    kind_(Kind::REAL),
    parent_(nullptr),
    left_(fakeLeaf),
    right_(fakeLeaf),
    data_(new pair_t(k, v))
  {}

  BSTNode(Key &&k, Value &&v, BSTNode *fakeLeaf):
    kind_(Kind::REAL),
    parent_(nullptr),
    left_(fakeLeaf),
    right_(fakeLeaf),
    data_(new pair_t(std::move(k), std::move(v)))
  {}

  ~BSTNode()
  {
    delete data_;
  }

  BSTNode(const BSTNode &) = delete;
  BSTNode &operator=(const BSTNode &) = delete;
