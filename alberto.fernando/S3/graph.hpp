#ifndef GRAPH_HPP
#define GRAPH_HPP
#include "hashtable.hpp"
#include "slist.hpp"
#include <algorithm>
#include <string>
#include <utility>
#include <vector>

namespace alberto {

using EdgeKey     = std::pair< std::string, std::string >;
using WeightList  = SList< unsigned >;
using EdgeTable   = HashTable< EdgeKey, WeightList, pair_hash, pair_equal >;
using VertexTable = HashTable< std::string, bool, xx_hash >;

struct Graph {
  VertexTable vertices{32};
  EdgeTable   edges{64};
  void ensureVertex(const std::string& v);
  void addEdge(const std::string& src, const std::string& dst, unsigned w);
  void cutEdge(const std::string& src, const std::string& dst, unsigned w);  // ← ADICIONAR ESTA LINHA
  bool hasVertex(const std::string& v) const;
};

using GraphTable = HashTable< std::string, Graph, xx_hash >;

template< class HT >
std::vector< std::string >
sortedKeys(const HT& ht)
{
  std::vector< std::string > keys;
  for (const auto& p : ht) {
    keys.push_back(p.first);
  }
  std::sort(keys.begin(), keys.end());
  return keys;
}

std::vector< std::string > tokenize(const std::string& line);
void loadGraphs(const std::string& filename, GraphTable& graphs);
void safeAddGraph(GraphTable& graphs, const std::string& name, Graph g);

}

#endif
