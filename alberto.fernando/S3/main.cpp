#include "commands.hpp"
#include "graph.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
  if (argc < 2) {
    std::cerr << "Error: filename argument required\n"
              << "Usage: " << argv[0] << " filename\n";
    return 1;
  }
  alberto::GraphTable graphs(32);
  try {
    alberto::loadGraphs(argv[1], graphs);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  alberto::HashTable< std::string, alberto::CmdFn, alberto::xx_hash > cmds(16);
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
    const std::string& cmdName = tok[0];
    if (!cmds.has(cmdName)) {
      std::cout << "<INVALID COMMAND>\n";
      continue;
    }
    try {
      cmds.get(cmdName)(graphs, tok);
    } catch (const std::exception&) {
      std::cout << "<INVALID COMMAND>\n";
    }
  }
  return 0;
}
