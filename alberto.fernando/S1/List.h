#ifndef LIST_H
#define LIST_H

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <limits>
#include "iter.h"

namespace alberto {
  template < class T >
  class List {
  public:
    struct Elem {
      T data_;
      Elem* next_;
      Elem* prev_;

      explicit Elem(const T& val, Elem* nxt = nullptr, Elem* prv = nullptr) :
        data_(val),
        next_(nxt),
        prev_(prv)
      {}

      explicit Elem(T&& val, Elem* nxt = nullptr, Elem* prv = nullptr) :
        data_(std::move(val)),
        next_(nxt),
        prev_(prv)
      {}
    };

  private:
    Elem* head_;
    Elem* tail_;
    size_t sz_;
  public:
    List() noexcept :
      head_(nullptr),
      tail_(nullptr),
      sz_(0)
    {}

    List(const List& other) :
      head_(nullptr),
      tail_(nullptr),
      sz_(0)
    {
      for (Elem* curr = other.head_; curr != nullptr; curr = curr->next_) {
        push_back(curr->data_);
      }
    }

    List(List&& other) noexcept :
      head_(other.head_),
      tail_(other.tail_),
      sz_(other.sz_)
    {
      other.head_ = nullptr;
      other.tail_ = nullptr;
      other.sz_ = 0;
    }

    ~List()
    {
      clear();
    }

    List& operator=(const List& other)
    {
      if (this != &other) {
        List tmp(other);
        swap(tmp);
      }
      return *this;
    }

    List& operator=(List&& other) noexcept
    {
      List tmp(std::move(other));
      swap(tmp);
      return *this;
    }

    void swap(List& other) noexcept
    {
      std::swap(head_, other.head_);
      std::swap(tail_, other.tail_);
      std::swap(sz_, other.sz_);
    }

    bool empty() const noexcept
    {
      return sz_ == 0;
    }

    size_t size() const noexcept
    {
      return sz_;
    }

    void clear()
    {
      while (!empty()) {
        pop_front();
      }
    }
