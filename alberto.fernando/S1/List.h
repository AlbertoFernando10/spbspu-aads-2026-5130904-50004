#ifndef LIST_H
#define LIST_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <limits>

namespace alberto {
  template <class T> class List;
  template <class T>
  class Iter {
    friend class List<T>;

  public:
    Iter() noexcept : ptr(nullptr) {}
    Iter(const Iter&) noexcept = default;
    T& operator*() const noexcept { return ptr->data; }
    T* operator->() const noexcept { return &(ptr->data); }
    Iter& operator++() noexcept {
      ptr = ptr->next;
      return *this;
    }
    Iter& operator--() noexcept {
      ptr = ptr->prev;
      return *this;
    }
    Iter operator++(int) noexcept {
      Iter tmp = *this;
      ptr = ptr->next;
      return tmp;
    }
    Iter operator--(int) noexcept {
      Iter tmp = *this;
      ptr = ptr->prev;
      return tmp;
    }
    bool operator==(const Iter& other) const noexcept { return ptr == other.ptr; }
    bool operator!=(const Iter& other) const noexcept { return ptr != other.ptr; }

  private:
    typename List<T>::Elem* ptr;
    explicit Iter(typename List<T>::Elem* p) noexcept : ptr(p) {}
  };

  template <class T>
  class CIter {
    friend class List<T>;

  public:
    CIter() noexcept : ptr(nullptr) {}
    CIter(const CIter&) noexcept = default;
    const T& operator*() const noexcept { return ptr->data; }
    const T* operator->() const noexcept { return &(ptr->data); }
    CIter& operator++() noexcept {
      ptr = ptr->next
      return *this;
    }
    CIter& operator--() noexcept {
      ptr = ptr->prev;
      return *this;
    }
    CIter operator++(int) noexcept {
      CIter tmp = *this;
      ptr = ptr->next;
      return tmp;
    }
    CIter operator--(int) noexcept {
      CIter tmp = *this;
      ptr = ptr->prev;
      return tmp;
    }
    bool operator==(const CIter& other) const noexcept { return ptr == other.ptr; }
    bool operator!=(const CIter& other) const noexcept { return ptr != other.ptr; }

  private:
    const typename List<T>::Elem* ptr;
    explicit CIter(const typename List<T>::Elem* p) noexcept : ptr(p) {}
  };

 template <class T>
  class List {
  public:
    struct Elem {
      T data;
      Elem* next;
      Elem* prev;
      explicit Elem(const T& val, Elem* nxt = nullptr, Elem* prv = nullptr)
        : data(val), next(nxt), prev(prv) {}
      explicit Elem(T&& val, Elem* nxt = nullptr, Elem* prv = nullptr)
        : data(std::move(val)), next(nxt), prev(prv) {}
    };

  private:
    Elem* head;
    Elem* tail;
    size_t sz;
