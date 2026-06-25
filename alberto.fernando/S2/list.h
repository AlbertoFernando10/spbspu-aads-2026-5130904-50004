#ifndef LIST_H
#define LIST_H

#include <stdexcept>
#include <utility>

namespace alberto {

template< class T >
class List {
  struct Node {
    T value;
    Node* next;
    explicit Node(T v, Node* n = nullptr);
  };
  Node* head_ = nullptr;
  Node* tail_ = nullptr;
  size_t size_ = 0;

public:
  List() = default;
  List(const List& o);
  List(List&& o) noexcept;
  List& operator=(List o) noexcept;
  ~List();

  void swap(List& o) noexcept;
  bool empty() const noexcept;
  size_t size() const noexcept;

  T& front();
  const T& front() const;
  T& back();
  const T& back() const;

  void push_front(T v);
  void push_back(T v);
  T pop_front();
  void clear() noexcept;
};

template< class T >
List< T >::Node::Node(T v, Node* n)
  : value(std::move(v)), next(n)
{}

template< class T >
List< T >::List(const List& o)
{
  for (Node* n = o.head_; n; n = n->next) {
    push_back(n->value);
  }
}

template< class T >
List< T >::List(List&& o) noexcept
  : head_(o.head_), tail_(o.tail_), size_(o.size_)
{
  o.head_ = o.tail_ = nullptr;
  o.size_ = 0;
}

template< class T >
List< T >& List< T >::operator=(List o) noexcept
{
  swap(o);
  return *this;
}

template< class T >
List< T >::~List()
{
  clear();
}

template< class T >
void List< T >::swap(List& o) noexcept
{
  std::swap(head_, o.head_);
  std::swap(tail_, o.tail_);
  std::swap(size_, o.size_);
}

template< class T >
bool List< T >::empty() const noexcept
{
  return size_ == 0;
}

template< class T >
size_t List< T >::size() const noexcept
{
  return size_;
}

template< class T >
T& List< T >::front()
{
  if (!head_) {
    throw std::out_of_range("List::front: empty list");
  }
  return head_->value;
}

template< class T >
const T& List< T >::front() const
{
  if (!head_) {
    throw std::out_of_range("List::front: empty list");
  }
  return head_->value;
}

template< class T >
T& List< T >::back()
{
  if (!tail_) {
    throw std::out_of_range("List::back: empty list");
  }
  return tail_->value;
}

template< class T >
const T& List< T >::back() const
{
  if (!tail_) {
    throw std::out_of_range("List::back: empty list");
  }
  return tail_->value;
}

template< class T >
void List< T >::push_front(T v)
{
  Node* n = new Node(std::move(v), head_);
  head_ = n;
  if (!tail_) {
    tail_ = n;
  }
  ++size_;
}

template< class T >
void List< T >::push_back(T v)
{
  Node* n = new Node(std::move(v));
  if (tail_) {
    tail_->next = n;
  } else {
    head_ = n;
  }
  tail_ = n;
  ++size_;
}

template< class T >
T List< T >::pop_front()
{
  if (!head_) {
    throw std::out_of_range("List::pop_front: empty list");
  }
  Node* n = head_;
  T val = std::move(n->value);
  head_ = head_->next;
  if (!head_) {
    tail_ = nullptr;
  }
  delete n;
  --size_;
  return val;
}

template< class T >
void List< T >::clear() noexcept
{
  while (head_) {
    Node* n = head_->next;
    delete head_;
    head_ = n;
  }
  tail_ = nullptr;
  size_ = 0;
}

}

#endif
