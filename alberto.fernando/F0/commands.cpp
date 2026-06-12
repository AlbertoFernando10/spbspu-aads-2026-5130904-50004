#include "commands.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::vector< std::string > alberto::tokenize(const std::string& line)
{
  std::vector< std::string > tokens;
  std::istringstream iss(line);
  std::string tok;
  while (iss >> tok) {
    tokens.push_back(tok);
  }
  return tokens;
}
