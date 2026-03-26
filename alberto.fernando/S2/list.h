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
