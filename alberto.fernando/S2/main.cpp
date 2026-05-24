#include "calculator.h"
#include "stack.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]) {
    using namespace alberto;

    std::istream* in = &std::cin;
    std::ifstream file;

    if (argc > 1) {
        file.open(argv[1]);
        if (!file.is_open()) {
            std::cerr << "error: cannot open file \"" << argv[1] << "\"\n";
            return 1;
        }
        in = &file;
    }

    Stack<long long> results;
    std::string line;

    while (std::getline(*in, line)) {
        if (line.empty() || line.find_first_not_of(" \t") == std::string::npos) {
            continue;
        }
        try {
            results.push(process_line(line));
        } catch (const std::exception& e) {
            std::cerr << "error: " << e.what() << "\n";
            return 1;
        }
    }
    if (!results.empty()) {
        bool first = true;
        while (!results.empty()) {
            if (!first) std::cout << ' ';
            std::cout << results.drop();
            first = false;
        }
        std::cout << '\n';
    }
    return 0;
}
