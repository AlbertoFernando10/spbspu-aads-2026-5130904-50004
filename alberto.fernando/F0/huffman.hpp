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
  ~CodingEntry()
  {
    destroyTree(root_);
  }

  CodingEntry(const CodingEntry&) = delete;
  CodingEntry& operator=(const CodingEntry&) = delete;

  CodingEntry(CodingEntry&& o) noexcept:
    codes_(std::move(o.codes_)),
    sourceName_(std::move(o.sourceName_)),
    uniqueChars_(o.uniqueChars_),
    originalBits_(o.originalBits_),
    compressedBits_(o.compressedBits_),
    entropy_(o.entropy_),
    root_(o.root_)
  {
    o.root_ = nullptr;
  }

  CodingEntry& operator=(CodingEntry&& o) noexcept
  {
    if (this != &o) {
      destroyTree(root_);
      codes_ = std::move(o.codes_);
      sourceName_ = std::move(o.sourceName_);
      uniqueChars_ = o.uniqueChars_;
      originalBits_ = o.originalBits_;
      compressedBits_ = o.compressedBits_;
      entropy_ = o.entropy_;
      root_ = o.root_;
      o.root_ = nullptr;
    }
    return *this;
  }

private:

  static void destroyTree(HuffNode* node)
  {
    if (!node) {
      return;
    }
    destroyTree(node->left_);
    destroyTree(node->right_);
    delete node;
  }
};
using TextTable = HashTable< std::string, TextEntry, xx_hash >;
using CodingTable = HashTable< std::string, CodingEntry*, xx_hash >;

inline HashTable< char, size_t, xx_hash > buildFreqTable(const std::string& text)
{
  HashTable< char, size_t, xx_hash > freq(64);
  for (char c : text) {
    if (freq.has(c)) {
      ++freq.get(c);
    } else {
      freq.add(c, 1);
    }
  }
  return freq;
}

inline void buildCodesHelper(HuffNode* node,
    const std::string& prefix,
    HashTable< char, std::string, xx_hash >& codes)
{
  if (!node) {
    return;
  }
  if (!node->left_ && !node->right_) {
    codes.add(node->ch_, prefix.empty() ? "0" : prefix);
    return;
  }
  buildCodesHelper(node->left_, prefix + "0", codes);
  buildCodesHelper(node->right_, prefix + "1", codes);
}

inline HuffNode* buildTree(const HashTable< char, size_t, xx_hash >& freq)
{
  std::priority_queue< HuffNode*, std::vector< HuffNode* >, NodeCmp > pq;
  for (const auto& p : freq) {
    pq.push(new HuffNode(p.first, p.second));
  }
  if (pq.empty()) {
    return nullptr;
  }
  while (pq.size() > 1) {
    HuffNode* const l = pq.top();
    pq.pop();
    HuffNode* const r = pq.top();
    pq.pop();
    pq.push(new HuffNode(l->freq_ + r->freq_, l, r));
  }
  return pq.top();
}

inline double calcEntropy(const HashTable< char, size_t, xx_hash >& freq,
    size_t total)
{
  double h = 0.0;
  for (const auto& p : freq) {
    if (p.second > 0) {
      const double prob = static_cast< double >(p.second) / total;
      h -= prob * (std::log(prob) / std::log(2.0));
    }
  }
  return h;
}

}

#endif
