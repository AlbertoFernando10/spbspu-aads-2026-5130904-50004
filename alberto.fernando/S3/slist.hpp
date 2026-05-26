#ifndef SLIST_HPP
#define SLIST_HPP
#include <cstddef>
#include <utility>

namespace alberto {

template< class T >
class SList {
  struct Node {
    T     data;
    Node* next = nullptr;

    template< class... Args >
    explicit Node(Args&&... args):
      data(std::forward< Args >(args)...)
    {}
  };

  Node*  head_ = nullptr;
  size_t size_ = 0;

public:

  SList() = default;

  SList(const SList& o)
  {
    Node** cur = &head_;
    for (const Node* n = o.head_; n; n = n->next) {
      *cur = new Node(n->data);
      cur  = &(*cur)->next;
      ++size_;
    }
  }

  SList(SList&& o) noexcept:
    head_(o.head_),
    size_(o.size_)
  {
    o.head_ = nullptr;
    o.size_ = 0;
  }

  SList& operator=(SList o) noexcept
  {
    swap(o);
    return *this;
  }

  ~SList()
  {
    clear();
  }

  void swap(SList& o) noexcept
  {
    std::swap(head_, o.head_);
    std::swap(size_, o.size_);
  }

  void push_front(const T& v)
  {
    Node* n = new Node(v);
    n->next = head_;
    head_   = n;
    ++size_;
  }

  void push_front(T&& v)
  {
    Node* n = new Node(std::move(v));
    n->next = head_;
    head_   = n;
    ++size_;
  }

  void push_back(const T& v)
  {
    Node* n = new Node(v);
    if (!head_) {
      head_ = n;
    } else {
      Node* cur = head_;
      while (cur->next) {
        cur = cur->next;
      }
      cur->next = n;
    }
    ++size_;
  }
  template< class Pred >
  bool remove_if(Pred pred)
  {
    Node** cur = &head_;
    bool removed = false;
    while (*cur) {
      if (pred((*cur)->data)) {
        Node* del = *cur;
        *cur = del->next;
        delete del;
        --size_;
        removed = true;
      } else {
        cur = &(*cur)->next;
      }
    }
    return removed;
  }

  void clear()
  {
    while (head_) {
      Node* n = head_->next;
      delete head_;
      head_ = n;
    }
    size_ = 0;
  }

  size_t size() const
  {
    return size_;
  }

  bool empty() const
  {
    return size_ == 0;
  }

  struct iterator {
    Node* n_;

    explicit iterator(Node* n):
      n_(n)
    {}

    T& operator*()
    {
      return n_->data;
    }

    T* operator->()
    {
      return &n_->data;
    }

    iterator& operator++()
    {
      n_ = n_->next;
      return *this;
    }

    bool operator==(iterator o) const
    {
      return n_ == o.n_;
    }

    bool operator!=(iterator o) const
    {
      return n_ != o.n_;
    }
  };

  struct const_iterator {
    const Node* n_;

    explicit const_iterator(const Node* n):
      n_(n)
    {}

    const_iterator(iterator it):
      n_(it.n_)
    {}

    const T& operator*() const
    {
      return n_->data;
    }

    const T* operator->() const
    {
      return &n_->data;
    }

    const_iterator& operator++()
    {
      n_ = n_->next;
      return *this;
    }

    bool operator==(const_iterator o) const
    {
      return n_ == o.n_;
    }

    bool operator!=(const_iterator o) const
    {
      return n_ != o.n_;
    }
  };

  iterator begin()
  {
    return iterator{head_};
  }

  iterator end()
  {
    return iterator{nullptr};
  }

  const_iterator begin() const
  {
    return const_iterator{head_};
  }

  const_iterator end() const
  {
    return const_iterator{nullptr};
  }

  const_iterator cbegin() const
  {
    return begin();
  }

  const_iterator cend() const
  {
    return end();
  }
};

}

#endif
