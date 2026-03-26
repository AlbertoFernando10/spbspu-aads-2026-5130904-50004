namespace alberto {

template<typename T>
class Stack {
    List<T> data_;

public:
    Stack()                      = default;
    Stack(const Stack&)          = default;
    Stack(Stack&&)               = default;
    Stack& operator=(const Stack&) = default;
    Stack& operator=(Stack&&)    = default;

    bool   empty() const noexcept { return data_.empty(); }
    size_t size()  const noexcept { return data_.size(); }

    void push(T v)  { data_.push_front(std::move(v)); }

    T drop() {
        if (empty()) throw std::out_of_range("Stack::drop: stack is empty");
        return data_.pop_front();
    }

    const T& top() const {
        if (empty()) throw std::out_of_range("Stack::top: stack is empty");
        return data_.front();
    }
    T& top() {
        if (empty()) throw std::out_of_range("Stack::top: stack is empty");
        return data_.front();
    }
};

}

#endif
