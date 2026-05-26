#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include "slist.hpp"
#include "xxhash.hpp"
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>

namespace alberto {

struct xx_hash {
  size_t operator()(const std::string& key) const
  {
    return static_cast< size_t >(
        xxhash_impl::xxhash32(key.data(), key.size()));
  }

  template< class T >
  size_t operator()(const T& key) const
  {
    return static_cast< size_t >(
        xxhash_impl::xxhash32(&key, sizeof(T)));
  }
};

struct pair_hash {
  size_t operator()(const std::pair< std::string, std::string >& p) const
  {
    const size_t h1 = xxhash_impl::xxhash32(
        p.first.data(), p.first.size());
    const size_t h2 = xxhash_impl::xxhash32(
        p.second.data(), p.second.size(), 0x9747b28cu);
    return h1 ^ (h2 * 2654435761ull);
  }
};

struct pair_equal {
  bool operator()(const std::pair< std::string, std::string >& a,
                  const std::pair< std::string, std::string >& b) const
  {
    return a == b;
  }
};

template< class Key, class Value,
          class Hash  = xx_hash,
          class Equal = std::equal_to< Key > >
class HashTable {
public:

  using Pair   = std::pair< Key, Value >;
  using Bucket = SList< Pair >;

private:

  Bucket* buckets_     = nullptr;
  size_t  numBuckets_  = 0;
  size_t  count_       = 0;
  size_t  bucketSize_  = 0;
  Hash    hash_;
  Equal   equal_;

  size_t bucketIndex(const Key& k) const
  {
    return hash_(k) % numBuckets_;
  }

  void destroy() noexcept
  {
    delete[] buckets_;
    buckets_    = nullptr;
    numBuckets_ = 0;
    count_      = 0;
  }

public:

  explicit HashTable(size_t slots     = 16,
                     size_t bucketCap = 0,
                     Hash   h         = Hash{},
                     Equal  eq        = Equal{}):
    numBuckets_(slots > 0 ? slots : 1),
    bucketSize_(bucketCap),
    hash_(h),
    equal_(eq)
  {
    buckets_ = new Bucket[numBuckets_];
  }

  HashTable(const HashTable& o):
    numBuckets_(o.numBuckets_),
    count_(o.count_),
    bucketSize_(o.bucketSize_),
    hash_(o.hash_),
    equal_(o.equal_)
  {
    buckets_ = new Bucket[numBuckets_];
    for (size_t i = 0; i < numBuckets_; ++i) {
      buckets_[i] = o.buckets_[i];
    }
  }

  HashTable(HashTable&& o) noexcept:
    buckets_(o.buckets_),
    numBuckets_(o.numBuckets_),
    count_(o.count_),
    bucketSize_(o.bucketSize_),
    hash_(std::move(o.hash_)),
    equal_(std::move(o.equal_))
  {
    o.buckets_    = nullptr;
    o.numBuckets_ = 0;
    o.count_      = 0;
  }

  HashTable& operator=(HashTable o) noexcept
  {
    swap(o);
    return *this;
  }

  ~HashTable()
  {
    destroy();
  }

  void swap(HashTable& o) noexcept
  {
    std::swap(buckets_,    o.buckets_);
    std::swap(numBuckets_, o.numBuckets_);
    std::swap(count_,      o.count_);
    std::swap(bucketSize_, o.bucketSize_);
    std::swap(hash_,       o.hash_);
    std::swap(equal_,      o.equal_);
  }

  void add(const Key& k, Value v)
  {
    const size_t idx = bucketIndex(k);
    Bucket&      b   = buckets_[idx];

    if (bucketSize_ > 0 && b.size() >= bucketSize_) {
      throw std::overflow_error(
          "HashTable: bucket overflow — call rehash()");
    }

    for (auto& p : b) {
      if (equal_(p.first, k)) {
        p.second = std::move(v);
        return;
      }
    }
    b.push_back({k, std::move(v)});
    ++count_;
  }

  Value drop(const Key& k)
  {
    const size_t idx   = bucketIndex(k);
    Bucket&      b     = buckets_[idx];
    Value        tmp{};
    bool         found = false;

    for (auto& p : b) {
      if (equal_(p.first, k)) {
        tmp   = p.second;
        found = true;
        break;
      }
    }
    if (!found) {
      throw std::out_of_range("HashTable::drop: key not found");
    }
    b.remove_if([&](const Pair& p) {
      return equal_(p.first, k);
    });
    --count_;
    return tmp;
  }

  bool has(const Key& k) const
  {
    const size_t  idx = bucketIndex(k);
    const Bucket& b   = buckets_[idx];
    for (const auto& p : b) {
      if (equal_(p.first, k)) {
        return true;
      }
    }
    return false;
  }

  Value& get(const Key& k)
  {
    const size_t idx = bucketIndex(k);
    Bucket&      b   = buckets_[idx];
    for (auto& p : b) {
      if (equal_(p.first, k)) {
        return p.second;
      }
    }
    throw std::out_of_range("HashTable::get: key not found");
  }

  const Value& get(const Key& k) const
  {
    const size_t  idx = bucketIndex(k);
    const Bucket& b   = buckets_[idx];
    for (const auto& p : b) {
      if (equal_(p.first, k)) {
        return p.second;
      }
    }
    throw std::out_of_range("HashTable::get: key not found");
  }

  void rehash(size_t slots)
  {
    if (slots == 0) {
      slots = 1;
    }
    HashTable tmp(slots, bucketSize_, hash_, equal_);
    for (size_t i = 0; i < numBuckets_; ++i) {
      for (auto& p : buckets_[i]) {
        tmp.add(p.first, p.second);
      }
    }
    swap(tmp);
  }

  size_t size() const
  {
    return count_;
  }

  bool empty() const
  {
    return count_ == 0;
  }

  size_t bucketCount() const
  {
    return numBuckets_;
  }

  double loadFactor() const
  {
    return numBuckets_
        ? static_cast< double >(count_) / numBuckets_
        : 0.0;
  }

  double avgPerBucket() const
  {
    return loadFactor();
  }

  size_t maxBucketSize() const
  {
    size_t mx = 0;
    for (size_t i = 0; i < numBuckets_; ++i) {
      if (buckets_[i].size() > mx) {
        mx = buckets_[i].size();
      }
    }
    return mx;
  }

  struct iterator {
    HashTable*              ht_;
    size_t                  bi_;
    typename Bucket::iterator it_;

    iterator(HashTable* ht, size_t bi, typename Bucket::iterator it):
      ht_(ht),
      bi_(bi),
      it_(it)
    {
      advanceToValid();
    }

    void advanceToValid()
    {
      while (bi_ < ht_->numBuckets_ && it_ == ht_->buckets_[bi_].end()) {
        ++bi_;
        if (bi_ < ht_->numBuckets_) {
          it_ = ht_->buckets_[bi_].begin();
        }
      }
    }

    Pair& operator*() { return *it_; }
    Pair* operator->() { return &*it_; }

    iterator& operator++()
    {
      ++it_;
      advanceToValid();
      return *this;
    }

    bool operator==(const iterator& o) const
    {
      return bi_ == o.bi_
          && (bi_ >= ht_->numBuckets_ || it_ == o.it_);
    }

    bool operator!=(const iterator& o) const { return !(*this == o); }
  };

  struct const_iterator {
    const HashTable*                  ht_;
    size_t                            bi_;
    typename Bucket::const_iterator   it_;

    const_iterator(const HashTable* ht, size_t bi,
                   typename Bucket::const_iterator it):
      ht_(ht),
      bi_(bi),
      it_(it)
    {
      advanceToValid();
    }

    void advanceToValid()
    {
      while (bi_ < ht_->numBuckets_ && it_ == ht_->buckets_[bi_].cend()) {
        ++bi_;
        if (bi_ < ht_->numBuckets_) {
          it_ = ht_->buckets_[bi_].cbegin();
        }
      }
    }

    const Pair& operator*() const { return *it_; }
    const Pair* operator->() const { return &*it_; }

    const_iterator& operator++()
    {
      ++it_;
      advanceToValid();
      return *this;
    }

    bool operator==(const const_iterator& o) const
    {
      return bi_ == o.bi_
          && (bi_ >= ht_->numBuckets_ || it_ == o.it_);
    }

    bool operator!=(const const_iterator& o) const { return !(*this == o); }
  };

  iterator begin()
  {
    if (numBuckets_ == 0) return end();
    return iterator(this, 0, buckets_[0].begin());
  }

  iterator end()
  {
    return iterator(this, numBuckets_,
        numBuckets_ ? buckets_[numBuckets_ - 1].end()
                    : typename Bucket::iterator(nullptr));
  }

  const_iterator begin() const
  {
    if (numBuckets_ == 0) return end();
    return const_iterator(this, 0, buckets_[0].cbegin());
  }

  const_iterator end() const
  {
    if (numBuckets_ == 0) {
      return const_iterator(this, 0,
          typename Bucket::const_iterator(nullptr));
    }
    return const_iterator(this, numBuckets_,
        buckets_[numBuckets_ - 1].cend());
  }

  const_iterator cbegin() const { return begin(); }
  const_iterator cend() const { return end(); }
};

}

#endif
