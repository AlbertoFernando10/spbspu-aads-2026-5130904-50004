#include <iostream>
#include <string>
#include "List.h"
using namespace alberto;
int main() {
  List<std::string> names;
  List<List<unsigned long long>> nums;
  std::string nome;
  while (std::cin >> nome) {
    names.push_back(nome);
    nums.push_back(List<unsigned long long>());
    unsigned long long valor;
    while (std::cin >> valor) {
      nums.back().push_back(valor);
      if (std::cin.peek() == '\n') {
        break;
      }
    }
    std::cin.clear();
  }
  if (names.empty()) {
    std::cout << "0\n";
    return 0;
  }
  bool first = true;
  for (auto it = names.cbegin(); it != names.cend(); ++it) {
    if (!first) std::cout << " ";
    std::cout << *it;
    first = false;
  }
  std::cout << "\n";
  size_t max_size = 0;
  for (auto it = nums.cbegin(); it != nums.cend(); ++it) {
    if (it->size() > max_size) {
      max_size = it->size();
    }
  }
  if (max_size == 0) {
    std::cout << "0\n";
    return 0;
  }
  List<unsigned long long> sums;
  try {
    for (size_t col = 0; col < max_size; ++col) {
      unsigned long long current_sum = 0;
      bool has_values = false;
      auto nums_it = nums.cbegin();
      for (size_t i = 0; i < nums.size(); ++i) {
        if (col < nums_it->size()) {
          alberto::sum(current_sum, (*nums_it)[col]);
          has_values = true;
        }
        ++nums_it;
      }
      if (has_values) {
        bool first_in_col = true;
        nums_it = nums.cbegin();
        for (size_t i = 0; i < nums.size(); ++i) {
          if (col < nums_it->size()) {
            if (!first_in_col) std::cout << " ";
            std::cout << (*nums_it)[col];
            first_in_col = false;
          }
          ++nums_it;
        }
        std::cout << "\n";
        sums.push_back(current_sum);
      }
    }
    first = true;
    for (auto it = sums.cbegin(); it != sums.cend(); ++it) {
      if (!first) std::cout << " ";
      std::cout << *it;
      first = false;
    }
    std::cout << "\n";
  } catch (const std::overflow_error&) {
    std::cerr << "Overflow\n";
    return 1;
  }
  return 0;
}
