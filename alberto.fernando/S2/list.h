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
    bool   empty() const noexcept { return size_ == 0; }
    size_t size()  const noexcept { return size_; }

    T& front() {
        if (!head_) throw std::out_of_range("List::front: empty list");
        return head_->value;
    }
    const T& front() const {
        if (!head_) throw std::out_of_range("List::front: empty list");
        return head_->value;
    }

    T& back() {
        if (!tail_) throw std::out_of_range("List::back: empty list");
        return tail_->value;
    }
    const T& back() const {
        if (!tail_) throw std::out_of_range("List::back: empty list");
        return tail_->value;
    }
    void push_front(T v) {
        Node* n = new Node(std::move(v), head_);
        head_ = n;
        if (!tail_) tail_ = n;
        ++size_;
    }

    void push_back(T v) {
        Node* n = new Node(std::move(v));
        if (tail_) tail_->next = n;
        else       head_ = n;
        tail_ = n;
        ++size_;
    }

    T pop_front() {
        if (!head_) throw std::out_of_range("List::pop_front: empty list");
        Node* n = head_;
        T val   = std::move(n->value);
        head_   = head_->next;
        if (!head_) tail_ = nullptr;
        delete n;
        --size_;
        return val;
    }

    void clear() noexcept {
        while (head_) {
            Node* n = head_->next;
            delete head_;
            head_ = n;
        }
        tail_ = nullptr;
        size_ = 0;
    }
};

}

#endif
