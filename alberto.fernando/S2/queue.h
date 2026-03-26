#define QUEUE_H

#include "list.h"

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

    void push(T v) { data_.push_back(std::move(v)); }

    T drop() {
        if (empty()) throw std::out_of_range("Queue::drop: queue is empty");
        return data_.pop_front();
    }

    const T& front() const {
        if (empty()) throw std::out_of_range("Queue::front: queue is empty");
        return data_.front();
    }
};

}

#endif
