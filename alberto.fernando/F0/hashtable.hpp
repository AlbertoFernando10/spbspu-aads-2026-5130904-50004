#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "xx_hash.hpp"

namespace alberto {

template <typename K, typename V, typename Hash = xx_hash>
class HashTable
{
public:
  using value_type = std::pair<K, V>;

  explicit HashTable(size_t initial_cap = 16)
    : m_buckets(initial_cap < 1 ? 16 : initial_cap)
  {
  }

  HashTable(HashTable&&) noexcept = default;
  HashTable& operator=(HashTable&&) noexcept = default;

  HashTable(const HashTable&) = default;
  HashTable& operator=(const HashTable&) = default;

  bool empty() const noexcept
  {
    return m_count == 0;
  }

  size_t size() const noexcept
  {
    return m_count;
  }

  bool has(const K& key) const
  {
    return findIndex(key) != m_buckets.size();
  }

  V& get(const K& key)
  {
    const size_t idx = findIndex(key);
    if (idx == m_buckets.size())
    {
      throw std::invalid_argument("HashTable::get: key not found");
    }
    return m_buckets[idx].kv.second;
  }

  const V& get(const K& key) const
  {
    const size_t idx = findIndex(key);
    if (idx == m_buckets.size())
    {
      throw std::invalid_argument("HashTable::get: key not found");
    }
    return m_buckets[idx].kv.second;
  }
