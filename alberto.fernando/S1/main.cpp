#include <iostream>
#include <string>
#include <climits>
#include "List.h"

using namespace alberto;

int main()
{
  List< std::string > names;
  List< List< unsigned long long > > nums;
  std::string nome;
  if (!(std::cin >> nome)) {
    std::cout << "0\n";
    return 0;
  }
  names.push_back(nome);
  nums.push_back(List< unsigned long long >());
  unsigned long long valor;
  while (std::cin >> valor) {
    nums.back().push_back(valor);
    if (std::cin.peek() == '\n') {
      break;
    }
  }
  std::cin.clear();
  while (std::cin >> nome) {
    names.push_back(nome);
    nums.push_back(List< unsigned long long >());
    while (std::cin >> valor) {
      nums.back().push_back(valor);
      if (std::cin.peek() == '\n') {
        break;
      }
    }
    std::cin.clear();
  }
  size_t max_size = 0;
  for (auto it = nums.cbegin(); it != nums.cend(); ++it) {
    if (it->size() > max_size) {
      max_size = it->size();
    }
  }
  if (max_size == 0) {
    auto names_it = names.cbegin();
    std::cout << *names_it;
    ++names_it;
    for (; names_it != names.cend(); ++names_it) {
      std::cout << " " << *names_it;
    }
    std::cout << "\n0\n";
    return 0;
  }
  auto names_it = names.cbegin();
  std::cout << *names_it;
  ++names_it;
  for (; names_it != names.cend(); ++names_it) {
    std::cout << " " << *names_it;
  }
  std::cout << "\n";
  for (size_t col = 0; col < max_size; ++col) {
    auto nums_it = nums.cbegin();
    bool printed = false;
    for (size_t i = 0; i < nums.size(); ++i) {
      if (col < nums_it->size()) {
        if (printed) {
          std::cout << " ";
        }
        std::cout << (*nums_it)[col];
        printed = true;
      }
      ++nums_it;
    }
    if (printed) {
      std::cout << "\n";
    }
  }
  List< unsigned long long > sums;
  for (size_t col = 0; col < max_size; ++col) {
    unsigned long long current_sum = 0;
    auto nums_it = nums.cbegin();
    for (size_t i = 0; i < nums.size(); ++i) {
      if (col < nums_it->size()) {
        unsigned long long val = (*nums_it)[col];
        if (current_sum > ULLONG_MAX - val) {
          std::cerr << "Overflow\n";
          return 1;
        }
        current_sum += val;
      }
      ++nums_it;
    }
    sums.push_back(current_sum);
  }
  auto sums_it = sums.cbegin();
  std::cout << *sums_it;
  ++sums_it;
  for (; sums_it != sums.cend(); ++sums_it) {
    std::cout << " " << *sums_it;
  }
  std::cout << "\n";
  return 0;
}
