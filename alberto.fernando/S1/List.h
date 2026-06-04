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
    T& front()
    {
      if (empty()) {
        throw std::out_of_range("Empty list");
      }
      return head_->data_;
    }

    const T& front() const
    {
      if (empty()) {
        throw std::out_of_range("Empty list");
      }
      return head_->data_;
    }

    T& back()
    {
      if (empty()) {
        throw std::out_of_range("Empty list");
      }
      return tail_->data_;
    }

    const T& back() const
    {
      if (empty()) {
        throw std::out_of_range("Empty list");
      }
      return tail_->data_;
    }

    T& operator[](size_t index)
    {
      if (index >= sz_) {
        throw std::out_of_range("Invalid index");
      }
      Elem* curr = head_;
      for (size_t i = 0; i < index; ++i) {
        curr = curr->next_;
      }
      return curr->data_;
    }

    const T& operator[](size_t index) const
    {
      if (index >= sz_) {
        throw std::out_of_range("Invalid index");
      }
      Elem* curr = head_;
      for (size_t i = 0; i < index; ++i) {
        curr = curr->next_;
      }
      return curr->data_;
    }

    Iter< T > begin() noexcept
    {
      return Iter< T >(head_);
    }

    Iter< T > end() noexcept
    {
      return Iter< T >(nullptr);
    }

    CIter< T > begin() const noexcept
    {
      return CIter< T >(head_);
    }

    CIter< T > end() const noexcept
    {
      return CIter< T >(nullptr);
    }

    CIter< T > cbegin() const noexcept
    {
      return CIter< T >(head_);
    }

    CIter< T > cend() const noexcept
    {
      return CIter< T >(nullptr);
    }
    void push_front(const T& value)
    {
      Elem* novo = new Elem(value, head_, nullptr);
      if (empty()) {
        tail_ = novo;
      } else {
        head_->prev_ = novo;
      }
      head_ = novo;
      sz_++;
    }

    void push_front(T&& value)
    {
      Elem* novo = new Elem(std::move(value), head_, nullptr);
      if (empty()) {
        tail_ = novo;
      } else {
        head_->prev_ = novo;
      }
      head_ = novo;
      sz_++;
    }

    void pop_front()
    {
      if (empty()) {
        return;
      }
      Elem* temp = head_;
      head_ = head_->next_;
      if (head_) {
        head_->prev_ = nullptr;
      } else {
        tail_ = nullptr;
      }
      delete temp;
      sz_--;
    }

    void push_back(const T& value)
    {
      Elem* novo = new Elem(value, nullptr, tail_);
      if (empty()) {
        head_ = novo;
      } else {
        tail_->next_ = novo;
      }
      tail_ = novo;
      sz_++;
    }

    void push_back(T&& value)
    {
      Elem* novo = new Elem(std::move(value), nullptr, tail_);
      if (empty()) {
        head_ = novo;
      } else {
        tail_->next_ = novo;
      }
      tail_ = novo;
      sz_++;
    }

    void pop_back()
    {
      if (empty()) {
        return;
      }
      Elem* temp = tail_;
      tail_ = tail_->prev_;
      if (tail_) {
        tail_->next_ = nullptr;
      } else {
        head_ = nullptr;
      }
      delete temp;
      sz_--;
    }
