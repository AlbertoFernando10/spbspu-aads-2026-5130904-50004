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
