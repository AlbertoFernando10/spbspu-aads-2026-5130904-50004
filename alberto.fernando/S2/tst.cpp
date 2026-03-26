#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char* argv[]);
#endif

#define BOOST_TEST_MODULE S2
#include <boost/test/included/unit_test.hpp>
#include <sstream>
#include "queue.h"
#include "stack.h"
#include "calculator.h"

using namespace alberto;

struct Data {
    bool is_num;
    long long num;
    char op;

    Data() : is_num(false), num(0), op(0) {}
    Data(long long n) : is_num(true), num(n), op(0) {}
    Data(char c) : is_num(false), num(0), op(c) {}

    bool is_int() const { return is_num; }
    long long value() const { return num; }
    char char_value() const { return op; }
};

Queue<Data> input(std::istream& in) {
    Queue<Data> result;
    std::string line;
    if (!std::getline(in, line)) return result;

    size_t pos = 0;
    while (pos <= line.size()) {
        size_t end = line.find(' ', pos);
        if (end == std::string::npos) end = line.size();
        std::string tok = line.substr(pos, end - pos);
        pos = end + 1;
        if (tok.empty()) continue;

        bool is_number = true;
        for (char c : tok) {
            if (!isdigit(c) && c != '-') {
                is_number = false;
                break;
            }
        }

        if (is_number) {
            result.push(Data(std::stoll(tok)));
        } else if (tok.size() == 1) {
            result.push(Data(tok[0]));
        }
    }
    return result;
}

void postfix(Queue<Data>& infix, Queue<Data>& postfix) {
    Stack<Data> ops;

    while (!infix.empty()) {
        Data tok = infix.drop();

        if (tok.is_int()) {
            postfix.push(tok);
        } else if (tok.op == '(') {
            ops.push(tok);
        } else if (tok.op == ')') {
            while (!ops.empty() && ops.top().op != '(')
                postfix.push(ops.drop());
            if (!ops.empty()) ops.drop();
        } else {
            int prec = (tok.op == '+' || tok.op == '-') ? 1 : 2;
            while (!ops.empty() && ops.top().op != '(' &&
                   ((ops.top().op == '+' || ops.top().op == '-') ? 1 : 2) >= prec)
                postfix.push(ops.drop());
            ops.push(tok);
        }
    }

    while (!ops.empty()) postfix.push(ops.drop());
}

int reverse(int n) {
    int rev = 0;
    while (n) {
        rev = rev * 10 + n % 10;
        n /= 10;
    }
    return rev;
}

BOOST_AUTO_TEST_CASE(main_test)
{
    Queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);

    Queue<int> new_q(q);
    BOOST_TEST(new_q.drop() == 10);

    Stack<int> stack;
    while (!q.empty()) {
        stack.push(q.drop());
    }
    BOOST_TEST(stack.drop() == 30);

    Queue<Data> q1, q2;
    std::stringstream in("2 + 3 * 4");
    std::stringstream out;

    q1 = input(in);
    postfix(q1, q2);

    while (!q2.empty()) {
        Data val = q2.drop();
        if (val.is_int()) {
            out << val.value();
        } else {
            out << val.op;
        }
    }
    BOOST_TEST(out.str() == "234*+");

    BOOST_TEST(reverse(12345) == 54321);
}
