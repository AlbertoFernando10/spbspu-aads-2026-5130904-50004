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
    Iter& operator++() noexcept
    {
      ptr = ptr->next_;
      return *this;
    }

    Iter& operator--() noexcept
    {
      ptr = ptr->prev_;
      return *this;
    }

    Iter operator++(int) noexcept
    {
      Iter tmp = *this;
      ptr = ptr->next_;
      return tmp;
    }

    Iter operator--(int) noexcept
    {
      Iter tmp = *this;
      ptr = ptr->prev_;
      return tmp;
    }

    bool operator==(const Iter& other) const noexcept
    {
      return ptr == other.ptr;
    }

    bool operator!=(const Iter& other) const noexcept
    {
      return ptr != other.ptr;
    }
  private:
    typename List< T >::Elem* ptr;

    explicit Iter(typename List< T >::Elem* p) noexcept :
      ptr(p)
    {}
  };
  template < class T >
  class CIter: public std::iterator< std::bidirectional_iterator_tag, T,
      std::ptrdiff_t, const T*, const T& > {
    friend class List< T >;

  public:
    CIter() noexcept :
      ptr(nullptr)
    {}

    CIter(const CIter&) noexcept = default;

    const T& operator*() const noexcept
    {
      return ptr->data_;
    }

    const T* operator->() const noexcept
    {
      return &(ptr->data_);
    }

    CIter& operator++() noexcept
    {
      ptr = ptr->next_;
      return *this;
    }

    CIter& operator--() noexcept
    {
      ptr = ptr->prev_;
      return *this;
    }

    CIter operator++(int) noexcept
    {
      CIter tmp = *this;
      ptr = ptr->next_;
      return tmp;
    }

    CIter operator--(int) noexcept
    {
      CIter tmp = *this;
      ptr = ptr->prev_;
      return tmp;
    }

    bool operator==(const CIter& other) const noexcept
    {
      return ptr == other.ptr;
    }

    bool operator!=(const CIter& other) const noexcept
    {
      return ptr != other.ptr;
    }

  private:
    const typename List< T >::Elem* ptr;

    explicit CIter(const typename List< T >::Elem* p) noexcept :
      ptr(p)
    {}
  };
}

#endif
