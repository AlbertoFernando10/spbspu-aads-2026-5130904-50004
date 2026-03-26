#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <string>
#include <stdexcept>
#include "queue.h"
#include "stack.h"

namespace alberto {

bool is_operator(const std::string& tok);
int precedence(const std::string& op);
long long apply_op(const std::string& op, long long a, long long b);
Queue<std::string> infix_to_postfix(const std::string& line);
long long eval_postfix(Queue<std::string>& pf);
long long process_line(const std::string& line);

}

#endif
