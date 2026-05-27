#ifndef COMMANDS_HPP
#define COMMANDS_HPP
#include "graph.hpp"
#include "hashtable.hpp"
#include <functional>
#include <string>
#include <vector>

namespace alberto {

using CmdFn = std::function< void(GraphTable&,
                                  const std::vector< std::string >&) >;

void cmdGraphs  (const GraphTable& graphs);
void cmdVertexes(const GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdOutbound(const GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdInbound (const GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdBind    (GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdCut     (GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdCreate  (GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdMerge   (GraphTable& graphs,
                 const std::vector< std::string >& tok);
void cmdExtract (GraphTable& graphs,
                 const std::vector< std::string >& tok);

void buildCommands(HashTable< std::string, CmdFn, xx_hash >& cmds);

}

#endif
