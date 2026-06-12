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
enum class TextState { RAW, ENCODED };

struct TextEntry {
  std::string content_;
  TextState state_;
  std::string sourceFile_;
  std::string codingName_;

  TextEntry():
    state_(TextState::RAW)
  {}

  TextEntry(const std::string& content,
      TextState state,
      const std::string& sourceFile,
      const std::string& codingName = ""):
    content_(content),
    state_(state),
    sourceFile_(sourceFile),
    codingName_(codingName)
  {}
};

struct CodingEntry {
  HashTable< char, std::string, xx_hash > codes_;
  std::string sourceName_;
  size_t uniqueChars_;
  size_t originalBits_;
  size_t compressedBits_;
  double entropy_;
  HuffNode* root_;

  CodingEntry():
    codes_(64),
    uniqueChars_(0),
    originalBits_(0),
    compressedBits_(0),
    entropy_(0.0),
    root_(nullptr)
  {}
