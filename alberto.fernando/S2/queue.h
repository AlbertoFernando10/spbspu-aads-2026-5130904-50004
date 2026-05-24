#ifndef QUEUE_H
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
};

}

#endif
