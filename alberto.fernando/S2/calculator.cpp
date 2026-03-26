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
Queue<std::string> infix_to_postfix(const std::string& line) {
    Queue<std::string> output;
    Stack<std::string> ops;

    size_t pos = 0;
    while (pos <= line.size()) {
        size_t end = line.find(' ', pos);
        if (end == std::string::npos) end = line.size();

        std::string tok = line.substr(pos, end - pos);
        pos = end + 1;

        if (tok.empty()) continue;

        if (tok == "(") {
            ops.push(tok);
        } else if (tok == ")") {
            while (!ops.empty() && ops.top() != "(") {
                output.push(ops.drop());
            }
            if (ops.empty())
                throw std::runtime_error("mismatched parentheses");
            ops.drop();
        } else if (is_operator(tok)) {
            while (!ops.empty() &&
                   ops.top() != "(" &&
                   precedence(ops.top()) >= precedence(tok)) {
                output.push(ops.drop());
            }
            ops.push(tok);
        } else {
            size_t i = 0;
            if (!tok.empty() && (tok[0] == '-' || tok[0] == '+')) ++i;
            bool valid = (i < tok.size());
            for (; i < tok.size(); ++i)
                if (tok[i] < '0' || tok[i] > '9') { valid = false; break; }
            if (!valid)
                throw std::runtime_error("invalid token: \"" + tok + "\"");
            output.push(tok);
        }
    }

    while (!ops.empty()) {
        if (ops.top() == "(" || ops.top() == ")")
            throw std::runtime_error("mismatched parentheses");
        output.push(ops.drop());
    }

    return output;
}
long long eval_postfix(Queue<std::string>& pf) {
    Stack<long long> stk;

    while (!pf.empty()) {
        std::string tok = pf.drop();
        if (is_operator(tok)) {
            if (stk.size() < 2)
                throw std::runtime_error("invalid expression (not enough operands)");
            long long b = stk.drop();
            long long a = stk.drop();
            stk.push(apply_op(tok, a, b));
        } else {
            bool neg  = false;
            size_t i  = 0;
            if (!tok.empty() && tok[0] == '-') { neg = true;  ++i; }
            else if (!tok.empty() && tok[0] == '+')            ++i;

            long long val = 0;
            for (; i < tok.size(); ++i) {
                if (tok[i] < '0' || tok[i] > '9')
                    throw std::runtime_error("invalid number: " + tok);
                val = val * 10 + (tok[i] - '0');
            }
            stk.push(neg ? -val : val);
        }
    }

    if (stk.size() != 1)
        throw std::runtime_error("invalid expression (leftover operands)");
    return stk.drop();
}
long long process_line(const std::string& line) {
    auto pf = infix_to_postfix(line);
    return eval_postfix(pf);
}

}
