#include "calculator.h"
#include <cstddef>

namespace alberto {

bool is_operator(const std::string& tok) {
    return tok == "+" || tok == "-" || tok == "*" ||
           tok == "/" || tok == "%" || tok == "&";
}

int precedence(const std::string& op) {
    if (op == "+" || op == "-") return 1;
    if (op == "*" || op == "/" || op == "%") return 2;
    if (op == "&") return 0;
    return -1;
}

long long apply_op(const std::string& op, long long a, long long b) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") {
        if (b == 0) throw std::runtime_error("division by zero");
        return a / b;
    }
    if (op == "%") {
        if (b == 0) throw std::runtime_error("modulo by zero");
        return a % b;
    }
    if (op == "&") return a & b;
    throw std::runtime_error("unknown operator: " + op);
}
