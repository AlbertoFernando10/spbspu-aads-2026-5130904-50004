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
public:
    List() noexcept : head(nullptr), tail(nullptr), sz(0) {}

    ~List() { clear(); }

    List(const List& other) : head(nullptr), tail(nullptr), sz(0) {
      for (Elem* curr = other.head; curr != nullptr; curr = curr->next) {
        push_back(curr->data);
      }
    }

    List(List&& other) noexcept
      : head(other.head), tail(other.tail), sz(other.sz) {
      other.head = nullptr;
      other.tail = nullptr;
      other.sz = 0;
    }

    List& operator=(const List& other) {
      if (this != &other) {
        List tmp(other);
        swap(tmp);
      }
      return *this;
    }

    List& operator=(List&& other) noexcept {
      if (this != &other) {
        clear();
        head = other.head;
        tail = other.tail;
        sz = other.sz;
        other.head = nullptr;
        other.tail = nullptr;
        other.sz = 0;
      }
      return *this;
    }

    void swap(List& other) noexcept {
      std::swap(head, other.head);
      std::swap(tail, other.tail);
      std::swap(sz, other.sz);
    }
Iter<T> begin() noexcept { return Iter<T>(head); }
    Iter<T> end() noexcept { return Iter<T>(nullptr); }
    CIter<T> begin() const noexcept { return CIter<T>(head); }
    CIter<T> end() const noexcept { return CIter<T>(nullptr); }
    CIter<T> cbegin() const noexcept { return CIter<T>(head); }
    CIter<T> cend() const noexcept { return CIter<T>(nullptr); }

    bool empty() const noexcept { return sz == 0; }
    size_t size() const noexcept { return sz; }

    T& front() {
      if (empty()) throw std::out_of_range("Empty list");
      return head->data;
    }

    const T& front() const {
      if (empty()) throw std::out_of_range("Empty list");
      return head->data;
    }

    T& back() {
      if (empty()) throw std::out_of_range("Empty list");
      return tail->data;
    }

    const T& back() const {
      if (empty()) throw std::out_of_range("Empty list");
      return tail->data;
    }

    T& operator[](size_t index) {
      if (index >= sz) throw std::out_of_range("Invalid index");
      Elem* curr = head;
      for (size_t i = 0; i < index; ++i) {
        curr = curr->next;
      }
      return curr->data;
    }

    const T& operator[](size_t index) const {
      if (index >= sz) throw std::out_of_range("Invalid index");
      Elem* curr = head;
      for (size_t i = 0; i < index; ++i) {
        curr = curr->next;
      }
      return curr->data;
    }
void push_front(const T& value) {
      Elem* novo = new Elem(value, head, nullptr);
      if (empty()) tail = novo;
      else head->prev = novo;
      head = novo;
      sz++;
    }
    void push_front(T&& value) {
      Elem* novo = new Elem(std::move(value), head, nullptr);
      if (empty()) tail = novo;
      else head->prev = novo;
      head = novo;
      sz++;
    }
    void pop_front() {
      if (empty()) return;
      Elem* temp = head;
      head = head->next;
      if (head) head->prev = nullptr;
      else tail = nullptr;
      delete temp;
      sz--;
    }
    void push_back(const T& value) {
      Elem* novo = new Elem(value, nullptr, tail);
      if (empty()) head = novo;
      else tail->next = novo;
      tail = novo;
      sz++;
    }

    void push_back(T&& value) {
      Elem* novo = new Elem(std::move(value), nullptr, tail);
      if (empty()) head = novo;
      else tail->next = novo;
      tail = novo;
      sz++;
    }

    void pop_back() {
      if (empty()) return;
      Elem* temp = tail;
      tail = tail->prev;
      if (tail) tail->next = nullptr;
      else head = nullptr;
      delete temp;
      sz--;
    }
void insert(size_t index, const T& value) {
      if (index > sz) throw std::out_of_range("Invalid index");
      if (index == 0) { push_front(value); return; }
      if (index == sz) { push_back(value); return; }

      Elem* atual = head;
      for (size_t i = 0; i < index; ++i) atual = atual->next;

      Elem* anterior = atual->prev;
      Elem* novo = new Elem(value, atual, anterior);
      anterior->next = novo;
      atual->prev = novo;
      sz++;
    }

    void insert(size_t index, T&& value) {
      if (index > sz) throw std::out_of_range("Invalid index");
      if (index == 0) { push_front(std::move(value)); return; }
      if (index == sz) { push_back(std::move(value)); return; }

      Elem* atual = head;
      for (size_t i = 0; i < index; ++i) atual = atual->next;
      Elem* anterior = atual->prev;
      Elem* novo = new Elem(std::move(value), atual, anterior);
      anterior->next = novo;
      atual->prev = novo;
      sz++;
    }
