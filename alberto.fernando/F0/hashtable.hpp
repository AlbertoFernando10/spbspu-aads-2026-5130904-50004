#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <utility>
#include <vector>

#include "xx_hash.hpp"

namespace alberto {

constexpr size_t DEFAULT_BUCKET_COUNT = 16;
constexpr double LOAD_FACTOR = 0.65;

template <typename K, typename V, typename Hash = xx_hash>
class HashTable
{
public:
  using value_type = std::pair<K, V>;

  enum class SlotState : uint8_t
  {
    EMPTY,
    OCCUPIED,
    DELETED
  };

  struct Slot
  {
    value_type m_kv;
    SlotState m_state = SlotState::EMPTY;

    Slot() = default;
    Slot(Slot&&) noexcept = default;
    Slot& operator=(Slot&&) noexcept = default;
    Slot(const Slot&) = delete;
    Slot& operator=(const Slot&) = delete;
  };

  explicit HashTable(size_t initial_cap = DEFAULT_BUCKET_COUNT) :
    m_buckets(initial_cap < 1 ? DEFAULT_BUCKET_COUNT : initial_cap),
    m_count(0)
  {}

  HashTable(HashTable&&) noexcept = default;
  HashTable& operator=(HashTable&&) noexcept = default;
  HashTable(const HashTable&) = delete;
  HashTable& operator=(const HashTable&) = delete;

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
    if (idx == m_buckets.size()) {
      throw std::invalid_argument("HashTable::get: key not found");
    }
    return m_buckets[idx].m_kv.second;
  }

  const V& get(const K& key) const
  {
    const size_t idx = findIndex(key);
    if (idx == m_buckets.size()) {
      throw std::invalid_argument("HashTable::get: key not found");
    }
    return m_buckets[idx].m_kv.second;
  }

  void add(const K& key, V value)
  {
    if (has(key)) {
      throw std::invalid_argument("HashTable::add: key already exists");
    }

    if (m_buckets.empty() ||
        static_cast<double>(m_count + 1) / m_buckets.size() > LOAD_FACTOR) {
      rehash(m_buckets.size() * 2 + 1);
    }

    insertInto(m_buckets, key, std::move(value));
    ++m_count;
  }

  V drop(const K& key)
  {
    const size_t idx = findIndex(key);
    if (idx == m_buckets.size()) {
      throw std::invalid_argument("HashTable::drop: key not found");
    }

    V value = std::move(m_buckets[idx].m_kv.second);
    m_buckets[idx].m_state = SlotState::DELETED;
    --m_count;
    return value;
  }
  class iterator
  {
  public:
    iterator(const std::vector<Slot>* buckets, size_t index) :
      m_buckets(buckets),
      m_index(index)
    {
      advance();
    }

    const value_type& operator*() const
    {
      return (*m_buckets)[m_index].m_kv;
    }

    const value_type* operator->() const
    {
      return &(*m_buckets)[m_index].m_kv;
    }

    iterator& operator++()
    {
      ++m_index;
      advance();
      return *this;
    }

    bool operator==(const iterator& other) const
    {
      return m_index == other.m_index;
    }

    bool operator!=(const iterator& other) const
    {
      return m_index != other.m_index;
    }

  private:
    const std::vector<Slot>* m_buckets;
    size_t m_index;

    void advance()
    {
      while (m_index < m_buckets->size() &&
             (*m_buckets)[m_index].m_state != SlotState::OCCUPIED) {
        ++m_index;
      }
    }
  };

  iterator begin() const
  {
    return iterator(&m_buckets, 0);
  }

  iterator end() const
  {
    return iterator(&m_buckets, m_buckets.size());
  }

private:
  std::vector<Slot> m_buckets;
  size_t m_count = 0;
  Hash m_hasher;
  void rehash(size_t new_capacity)
  {
    std::vector<Slot> new_buckets(new_capacity);
    size_t new_count = 0;

    for (auto& slot : m_buckets) {
      if (slot.m_state == SlotState::OCCUPIED) {
        insertInto(new_buckets,
                   std::move(slot.m_kv.first),
                   std::move(slot.m_kv.second));
        ++new_count;
      }
    }

    m_buckets.swap(new_buckets);
    m_count = new_count;
  }
  void insertInto(std::vector<Slot>& buckets, K&& key, V&& value)
  {
    const size_t capacity = buckets.size();
    size_t index = m_hasher(key) % capacity;

    for (size_t i = 0; i < capacity; ++i) {
      SlotState state = buckets[index].m_state;
      if (state == SlotState::EMPTY || state == SlotState::DELETED) {
        buckets[index].m_kv = value_type(std::move(key), std::move(value));
        buckets[index].m_state = SlotState::OCCUPIED;
        return;
      }
      index = (index + 1) % capacity;
    }
    throw std::runtime_error("HashTable: table is full");
  }
  void insertInto(std::vector<Slot>& buckets, const K& key, V&& value)
  {
    K keyCopy = key;
    insertInto(buckets, std::move(keyCopy), std::move(value));
  }

  size_t findIndex(const K& key) const
  {
    const size_t capacity = m_buckets.size();
    if (capacity == 0) {
      return capacity;
    }

    size_t index = m_hasher(key) % capacity;
    for (size_t i = 0; i < capacity; ++i) {
      const SlotState state = m_buckets[index].m_state;
      if (state == SlotState::EMPTY) {
        return capacity;
      }
      if (state == SlotState::OCCUPIED && m_buckets[index].m_kv.first == key) {
        return index;
      }
      index = (index + 1) % capacity;
    }
    return capacity;
  }
};

}

#endif
