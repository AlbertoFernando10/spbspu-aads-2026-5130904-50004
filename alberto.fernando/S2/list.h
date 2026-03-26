#ifndef LIST_H
#define LIST_H

#include <stdexcept>
#include <utility>

namespace alberto {

template<class T>
class List {
    struct Node {
        T     value;
        Node* next;
        explicit Node(T v, Node* n = nullptr) : value(std::move(v)), next(n) {}
    };
    Node*  head_  = nullptr;
    Node*  tail_  = nullptr;
    size_t size_  = 0;

public:
    List() = default;
    List(const List& o) {
        for (Node* n = o.head_; n; n = n->next)
            push_back(n->value);
    }
    List(List&& o) noexcept
        : head_(o.head_), tail_(o.tail_), size_(o.size_) {
        o.head_ = o.tail_ = nullptr;
        o.size_ = 0;
    }
    List& operator=(List o) noexcept {
        swap(o);
        return *this;
    }
    ~List() { clear(); }

    void swap(List& o) noexcept {
        std::swap(head_, o.head_);
        std::swap(tail_, o.tail_);
        std::swap(size_, o.size_);
    }
