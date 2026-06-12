#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include "hashtable.hpp"
#include <cmath>
#include <cstddef>
#include <queue>
#include <string>
#include <vector>

namespace alberto {
struct HuffNode {
  char ch_;
  size_t freq_;
  HuffNode* left_;
  HuffNode* right_;

  HuffNode(char c, size_t f):
    ch_(c),
    freq_(f),
    left_(nullptr),
    right_(nullptr)
  {}

  HuffNode(size_t f, HuffNode* l, HuffNode* r):
    ch_('\0'),
    freq_(f),
    left_(l),
    right_(r)
  {}
};

struct NodeCmp {
  bool operator()(const HuffNode* a, const HuffNode* b) const
  {
    return a->freq_ > b->freq_;
  }
};
