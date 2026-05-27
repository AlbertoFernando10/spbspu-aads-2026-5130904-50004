#include "commands.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace {

using alberto::InnerTree;
using alberto::OuterTree;

void cmdPrint(const std::string &name, const OuterTree &datasets)
{
  const auto it = datasets.find(name);
  if (it == datasets.end()) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const InnerTree &tree = it->second;
  if (tree.empty()) {
    std::cout << "<EMPTY>\n";
    return;
  }

  std::cout << name;
  for (const auto &kv : tree) {
    std::cout << ' ' << kv.first << ' ' << kv.second;
  }
  std::cout << '\n';
}

void cmdComplement(
    const std::string &newName,
    const std::string &n1,
    const std::string &n2,
    OuterTree &datasets)
{
  const auto it1 = datasets.find(n1);
  const auto it2 = datasets.find(n2);
  if (it1 == datasets.end() || it2 == datasets.end()) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const InnerTree t1 = it1->second;
  const InnerTree t2 = it2->second;

  InnerTree result;
  for (const auto &kv : t1) {
    if (!t2.has(kv.first)) {
      result.push(kv.first, kv.second);
    }
  }

  datasets.push(newName, std::move(result));
}

void cmdIntersect(
    const std::string &newName,
    const std::string &n1,
    const std::string &n2,
    OuterTree &datasets)
{
  const auto it1 = datasets.find(n1);
  const auto it2 = datasets.find(n2);
  if (it1 == datasets.end() || it2 == datasets.end()) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const InnerTree t1 = it1->second;
  const InnerTree t2 = it2->second;

  InnerTree result;
  for (const auto &kv : t1) {
    if (t2.has(kv.first)) {
      result.push(kv.first, kv.second);
    }
  }

  datasets.push(newName, std::move(result));
}

void cmdUnion(
    const std::string &newName,
    const std::string &n1,
    const std::string &n2,
    OuterTree &datasets)
{
  const auto it1 = datasets.find(n1);
  const auto it2 = datasets.find(n2);
  if (it1 == datasets.end() || it2 == datasets.end()) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const InnerTree t1 = it1->second;
  const InnerTree t2 = it2->second;

  InnerTree result;
  for (const auto &kv : t2) {
    result.push(kv.first, kv.second);
  }
  for (const auto &kv : t1) {
    result.push(kv.first, kv.second);
  }

  datasets.push(newName, std::move(result));
}

}

namespace alberto {

void loadFile(const std::string &filename, OuterTree &datasets)
{
  std::ifstream fin(filename);
  if (!fin) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  while (std::getline(fin, line)) {
    if (line.empty()) {
      continue;
    }
    std::istringstream iss(line);

    std::string name;
    if (!(iss >> name)) {
      continue;
    }

    InnerTree tree;
    int key = 0;
    std::string val;
    while (iss >> key >> val) {
      tree.push(key, val);
    }

    datasets.push(name, std::move(tree));
  }
}

void repl(OuterTree &datasets)
{
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    std::istringstream iss(line);

    std::string cmd;
    iss >> cmd;

    if (cmd == "print") {
      std::string name;
      if (!(iss >> name)) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }
      cmdPrint(name, datasets);

    } else if (cmd == "complement") {
      std::string newName, n1, n2;
      if (!(iss >> newName >> n1 >> n2)) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }
      cmdComplement(newName, n1, n2, datasets);

    } else if (cmd == "intersect") {
      std::string newName, n1, n2;
      if (!(iss >> newName >> n1 >> n2)) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }
      cmdIntersect(newName, n1, n2, datasets);

    } else if (cmd == "union") {
      std::string newName, n1, n2;
      if (!(iss >> newName >> n1 >> n2)) {
        std::cout << "<INVALID COMMAND>\n";
        continue;
      }
      cmdUnion(newName, n1, n2, datasets);

    } else {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
}

}
