#ifndef QUEUE_H
#define QUEUE_H

#include "list.h"
#include <cstddef>
#include <utility>
#include <stdexcept>

namespace alberto {

template<typename T>
class Queue {
    List<T> data_;

public:
    Queue()                       = default;
    Queue(const Queue&)           = default;
    Queue(Queue&&)                = default;
    Queue& operator=(const Queue&) = default;
    Queue& operator=(Queue&&)     = default;

    bool   empty() const noexcept { return data_.empty(); }
    size_t size()  const noexcept { return data_.size(); }

    void push(const T& value) {
        data_.push_back(value);
    }

    void push(T&& value) {
        data_.push_back(std::move(value));
    }

    T drop() {
        if (empty()) throw std::out_of_range("Queue::drop: queue is empty");
        return data_.pop_front();
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("Queue::front: queue is empty");
        return data_.front();
    }

    T& front() {
        if (empty()) throw std::out_of_range("Queue::front: queue is empty");
        return data_.front();
    }
};

}

#endif
