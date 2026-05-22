#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "bstree.hpp"

#include <string>

namespace alberto {

using InnerTree = BSTree< int, std::string >;
using OuterTree = BSTree< std::string, InnerTree >;

void loadFile(const std::string &filename, OuterTree &datasets);
void repl(OuterTree &datasets);

}

#endif
