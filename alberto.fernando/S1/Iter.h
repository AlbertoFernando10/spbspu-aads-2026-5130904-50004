#ifndef ITER_H
#define ITER_H

#include <cstddef>
#include <iterator>
namespace alberto {
  template < class T > class List;

  template < class T >
  class Iter: public std::iterator< std::bidirectional_iterator_tag, T > {
    friend class List< T >;
  public:
    Iter() noexcept :
      ptr(nullptr)
    {}

    Iter(const Iter&) noexcept = default;

    T& operator*() const noexcept
    {
      return ptr->data_;
    }

    T* operator->() const noexcept
    {
      return &(ptr->data_);
    }
