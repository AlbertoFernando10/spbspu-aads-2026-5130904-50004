#include "commands.hpp"
#include <iostream>
#include <string>

int main()
{
  alberto::Session session;
  alberto::HashTable< std::string, alberto::CmdFn, alberto::xx_hash > cmds(32);
  alberto::buildCommands(cmds);
