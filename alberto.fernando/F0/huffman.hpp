#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include "hashtable.hpp"
#include <cstddef>
#include <memory>
#include <string>

namespace alberto {

constexpr size_t CODING_HASH_CAP = 64;

struct HuffNode
{
  char m_ch;
  size_t m_freq;
  std::unique_ptr< HuffNode > m_left;
  std::unique_ptr< HuffNode > m_right;

  HuffNode(char c, size_t f) :
    m_ch(c),
    m_freq(f),
    m_left(nullptr),
    m_right(nullptr)
  {}

  HuffNode(size_t f, std::unique_ptr< HuffNode > l, std::unique_ptr< HuffNode > r) :
    m_ch('\0'),
    m_freq(f),
    m_left(std::move(l)),
    m_right(std::move(r))
  {}
};

struct NodeCmp
{
  bool operator()(const HuffNode * a, const HuffNode * b) const;
};

enum class TextState
{
  RAW,
  ENCODED
};

struct TextEntry
{
  std::string m_content;
  TextState m_state;
  std::string m_sourceFile;
  std::string m_codingName;

  TextEntry() :
    m_state(TextState::RAW)
  {}

  TextEntry(const std::string & content,
            TextState state,
            const std::string & sourceFile,
            const std::string & codingName = "") :
    m_content(content),
    m_state(state),
    m_sourceFile(sourceFile),
    m_codingName(codingName)
  {}
  const std::string & getContent() const
  {
    return m_content;
  }

  TextState getState() const
  {
    return m_state;
  }

  const std::string & getSourceFile() const
  {
    return m_sourceFile;
  }

  const std::string & getCodingName() const
  {
    return m_codingName;
  }

  std::string getInfo() const;
};

struct CodingEntry
{
  HashTable< char, std::string, xx_hash > m_codes;
  std::string m_sourceName;
  size_t m_uniqueChars;
  size_t m_originalBits;
  size_t m_compressedBits;
  double m_entropy;
  std::unique_ptr< HuffNode > m_root;

  CodingEntry() :
    m_codes(CODING_HASH_CAP),
    m_uniqueChars(0),
    m_originalBits(0),
    m_compressedBits(0),
    m_entropy(0.0),
    m_root(nullptr)
  {}

  ~CodingEntry() = default;

  CodingEntry(const CodingEntry &) = delete;
  CodingEntry & operator=(const CodingEntry &) = delete;

  CodingEntry(CodingEntry && o) noexcept = default;
  CodingEntry & operator=(CodingEntry && o) noexcept = default;
  const HashTable< char, std::string, xx_hash > & getCodes() const
  {
    return m_codes;
  }

  const std::string & getSourceName() const
  {
    return m_sourceName;
  }

  size_t getUniqueChars() const
  {
    return m_uniqueChars;
  }

  size_t getOriginalBits() const
  {
    return m_originalBits;
  }

  size_t getCompressedBits() const
  {
    return m_compressedBits;
  }

  double getEntropy() const
  {
    return m_entropy;
  }

  const HuffNode * getRoot() const
  {
    return m_root.get();
  }
  std::string getCodesInfo() const;
  std::string getAnalysisInfo() const;
  std::string getComparisonInfo(const CodingEntry & other) const;
};

using TextTable = HashTable< std::string, TextEntry, xx_hash >;
using CodingTable = HashTable< std::string, std::unique_ptr< CodingEntry >, xx_hash >;

HashTable< char, size_t, xx_hash > buildFreqTable(const std::string & text);

void buildCodesHelper(const HuffNode * node,
                      const std::string & prefix,
                      HashTable< char, std::string, xx_hash > & codes);

std::unique_ptr< HuffNode > buildTree(const HashTable< char, size_t, xx_hash > & freq);

double calcEntropy(const HashTable< char, size_t, xx_hash > & freq,
                   size_t total);

std::string encodeToBits(const std::string & content,
                         const CodingEntry * const coding);

std::string decodeFromBits(const std::string & encoded,
                           const HuffNode * const root);

}

#endif
