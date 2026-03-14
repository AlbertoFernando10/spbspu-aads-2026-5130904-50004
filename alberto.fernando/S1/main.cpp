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
