#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "huffman.hpp"
#include <functional>
#include <string>
#include <vector>

namespace alberto {

struct Session {
  TextTable texts;
  CodingTable codings;

  Session():
    texts(32),
    codings(32)
  {}

  ~Session()
  {
    for (auto& p : codings) {
      delete p.second;
    }
  }
};
using CmdFn = std::function< void(Session&, const std::vector< std::string >&) >;

std::vector< std::string > tokenize(const std::string& line);
