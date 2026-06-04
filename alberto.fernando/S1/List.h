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
