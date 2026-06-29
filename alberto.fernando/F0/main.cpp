#include "commands.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::vector<std::string> tokenize(const std::string& line) {
  std::vector<std::string> tokens;
  std::istringstream iss(line);
  std::string token;
  while (iss >> token) tokens.push_back(token);
  return tokens;
}
}

int main() {
  alberto::Session session;
  alberto::HashTable<std::string, alberto::CmdFn, alberto::xx_hash> commands;
  alberto::buildCommands(commands);

  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    auto tokens = tokenize(line);
    if (tokens.empty()) continue;

    const std::string& cmdName = tokens[0];
    if (!commands.has(cmdName)) {
      std::cerr << "Unknown command: " << cmdName << std::endl;
      continue;
    }

    try {
      commands.get(cmdName)(session, tokens);
    } catch (const std::exception& e) {
      std::cerr << "Error: " << e.what() << std::endl;
      return 1;
    }
  }
  return 0;
}
