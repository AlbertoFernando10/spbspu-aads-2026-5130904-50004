#include "commands.hpp"
#include <iostream>
#include <string>

int main()
{
  alberto::Session session;
  alberto::HashTable< std::string, alberto::CmdFn, alberto::xx_hash > cmds(32);
  alberto::buildCommands(cmds);
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    const auto tok = alberto::tokenize(line);
    if (tok.empty()) {
      continue;
    }
