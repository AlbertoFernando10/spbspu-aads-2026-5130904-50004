#include "huffman.hpp"
#include <cmath>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <iomanip>

namespace alberto {

constexpr size_t INITIAL_HASH_CAP = 64;

bool NodeCmp::operator()(const HuffNode * a, const HuffNode * b) const
{
  return a->m_freq > b->m_freq;
}

static void drainQueue(
    std::priority_queue< HuffNode *, std::vector< HuffNode * >, NodeCmp > & pq)
{
  while (!pq.empty()) {
    delete pq.top();
    pq.pop();
  }
}

std::string TextEntry::getInfo() const
{
  std::ostringstream oss;
  if (m_state == TextState::RAW) {
    oss << "TEXT: " << m_content.substr(0, 30)
        << ", STATE: raw"
        << ", FILE: " << m_sourceFile;
  } else {
    oss << "TEXT: " << m_content.substr(0, 30)
        << ", STATE: encoded"
        << ", CODING: " << m_codingName;
  }
  return oss.str();
}

std::string CodingEntry::getCodesInfo() const
{
  std::ostringstream oss;
  oss << "HUFFMAN CODES: " << m_sourceName << "\n";
  for (const auto & p : m_codes) {
    oss << "'" << p.first << "': code=" << p.second << "\n";
  }
  return oss.str();
}

std::string CodingEntry::getAnalysisInfo() const
{
  std::ostringstream oss;
  const double ratio = (m_originalBits > 0)
      ? 100.0 * (1.0 - static_cast< double >(m_compressedBits) / m_originalBits)
      : 0.0;

  oss << "COMPRESSION ANALYSIS: " << m_sourceName << "\n"
      << "ORIGINAL SIZE:   " << m_originalBits << " bits\n"
      << "COMPRESSED SIZE: " << m_compressedBits << " bits\n"
      << std::fixed << std::setprecision(2)
      << "RATIO:           " << ratio << "% reduction\n"
      << "ENTROPY:         " << m_entropy << " bits/char";
  return oss.str();
}

std::string CodingEntry::getComparisonInfo(const CodingEntry & other) const
{
  std::ostringstream oss;
  const auto ratio = [](const CodingEntry & c) -> double {
    return (c.m_originalBits > 0)
        ? 100.0 * (1.0 - static_cast< double >(c.m_compressedBits) / c.m_originalBits)
        : 0.0;
  };

  const double r1 = ratio(*this);
  const double r2 = ratio(other);

  oss << "COMPRESSION COMPARISON\n"
      << std::fixed << std::setprecision(2)
      << m_sourceName
      << " | ORIGINAL: " << m_originalBits << " bits"
      << " | COMPRESSED: " << m_compressedBits << " bits"
      << " | RATIO: " << r1 << "% reduction\n"
      << other.m_sourceName
      << " | ORIGINAL: " << other.m_originalBits << " bits"
      << " | COMPRESSED: " << other.m_compressedBits << " bits"
      << " | RATIO: " << r2 << "% reduction";
  return oss.str();
}

HashTable< char, size_t, xx_hash > buildFreqTable(const std::string & text)
{
  HashTable< char, size_t, xx_hash > freq(INITIAL_HASH_CAP);
  for (char c : text) {
    if (freq.has(c)) {
      ++freq.get(c);
    } else {
      freq.add(c, 1);
    }
  }
  return freq;
}

void buildCodesHelper(const HuffNode * node,
                      const std::string & prefix,
                      HashTable< char, std::string, xx_hash > & codes)
{
  if (!node) {
    return;
  }
  if (!node->m_left && !node->m_right) {
    codes.add(node->m_ch, prefix.empty() ? "0" : prefix);
    return;
  }
  buildCodesHelper(node->m_left.get(), prefix + "0", codes);
  buildCodesHelper(node->m_right.get(), prefix + "1", codes);
}

std::unique_ptr< HuffNode > buildTree(const HashTable< char, size_t, xx_hash > & freq)
{
  std::priority_queue< HuffNode *, std::vector< HuffNode * >, NodeCmp > pq;

  for (const auto & p : freq) {
    HuffNode * node = nullptr;
    try {
      node = new HuffNode(p.first, p.second);
    } catch (...) {
      drainQueue(pq);
      throw;
    }
    try {
      pq.push(node);
    } catch (...) {
      delete node;
      drainQueue(pq);
      throw;
    }
  }

  if (pq.empty()) {
    return nullptr;
  }

  while (pq.size() > 1) {
    HuffNode * const l = pq.top(); pq.pop();
    HuffNode * const r = pq.top(); pq.pop();

    std::unique_ptr< HuffNode > parent;
    try {
      parent = std::make_unique< HuffNode >(
          l->m_freq + r->m_freq,
          std::unique_ptr< HuffNode >(l),
          std::unique_ptr< HuffNode >(r));
    } catch (...) {
      delete l; delete r;
      drainQueue(pq);
      throw;
    }

    try {
      pq.push(parent.get());
    } catch (...) {
      drainQueue(pq);
      throw;
    }
    parent.release();
  }

  return std::unique_ptr< HuffNode >(pq.top());
}

double calcEntropy(const HashTable< char, size_t, xx_hash > & freq, size_t total)
{
  double h = 0.0;
  for (const auto & p : freq) {
    if (p.second > 0) {
      const double prob = static_cast< double >(p.second) / total;
      h -= prob * (std::log(prob) / std::log(2.0));
    }
  }
  return h;
}

std::string encodeToBits(const std::string & content,
                         const CodingEntry * const coding)
{
  std::string bitstream;
  for (char c : content) {
    if (!coding->m_codes.has(c)) {
      throw std::invalid_argument("encode: character not in coding");
    }
    bitstream += coding->m_codes.get(c);
  }

  const size_t numBits = bitstream.size();
  std::string result(sizeof(size_t), '\0');
  for (size_t i = 0; i < sizeof(size_t); ++i) {
    result[i] = static_cast< char >((numBits >> (i * 8)) & 0xFF);
  }

  size_t i = 0;
  while (i < numBits) {
    unsigned char byte = 0;
    for (int bit = 7; bit >= 0 && i < numBits; --bit, ++i) {
      if (bitstream[i] == '1') {
        byte |= static_cast< unsigned char >(1 << bit);
      }
    }
    result += static_cast< char >(byte);
  }
  return result;
}

std::string decodeFromBits(const std::string & encoded,
                           const HuffNode * const root)
{
  if (!root) {
    throw std::invalid_argument("decode: empty coding tree");
  }
  if (encoded.size() < sizeof(size_t)) {
    throw std::invalid_argument("decode: invalid encoded data");
  }

  size_t numBits = 0;
  for (size_t i = 0; i < sizeof(size_t); ++i) {
    numBits |= static_cast< size_t >(
        static_cast< unsigned char >(encoded[i])) << (i * 8);
  }

  std::string result;
  if (!root->m_left && !root->m_right) {
    for (size_t i = 0; i < numBits; ++i) {
      result += root->m_ch;
    }
    return result;
  }

  const HuffNode * cur = root;
  size_t bitsRead = 0;
  const size_t dataStart = sizeof(size_t);

  for (size_t byteIdx = dataStart; byteIdx < encoded.size(); ++byteIdx) {
    const unsigned char byte = static_cast< unsigned char >(encoded[byteIdx]);
    for (int bit = 7; bit >= 0 && bitsRead < numBits; --bit, ++bitsRead) {
      const bool isOne = static_cast< bool >((byte >> bit) & 1);
      cur = isOne ? cur->m_right.get() : cur->m_left.get();
      if (!cur) {
        throw std::invalid_argument("decode: invalid bit sequence");
      }
      if (!cur->m_left && !cur->m_right) {
        result += cur->m_ch;
        cur = root;
      }
    }
  }
  if (cur != root) {
    throw std::invalid_argument("decode: incomplete bit sequence");
  }
  return result;
}

}
