#include "commands.hpp"

#include <iostream>
#include <stdexcept>

int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <data-file>\n";
    return 1;
  }

  alberto::OuterTree datasets;

  try {
    alberto::loadFile(argv[1], datasets);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }

  alberto::repl(datasets);
  return 0;
}
