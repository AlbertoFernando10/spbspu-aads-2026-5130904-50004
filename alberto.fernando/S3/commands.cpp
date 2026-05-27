#include "commands.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

void alberto::cmdGraphs(const GraphTable& graphs)
{
  const auto keys = sortedKeys(graphs);
  for (const auto& k : keys) {
    std::cout << k << "\n";
  }
  if (keys.empty()) {
    std::cout << "\n";
  }
}

void alberto::cmdVertexes(const GraphTable& graphs,
                           const std::vector< std::string >& tok)
{
  if (tok.size() < 2 || !graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  const Graph& g    = graphs.get(tok[1]);
  const auto   keys = sortedKeys(g.vertices);
  for (const auto& k : keys) {
    std::cout << k << "\n";
  }
  if (keys.empty()) {
    std::cout << "\n";
  }
}

void alberto::cmdOutbound(const GraphTable& graphs,
                           const std::vector< std::string >& tok)
{
  if (tok.size() < 3 || !graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  const Graph& g = graphs.get(tok[1]);
  if (!g.hasVertex(tok[2])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const std::string& src = tok[2];
  std::vector< std::pair< std::string, std::vector< unsigned > > > out;

  for (const auto& ep : g.edges) {
    if (ep.first.first == src) {
      std::vector< unsigned > ws;
      for (unsigned w : ep.second) {
        ws.push_back(w);
      }
      std::sort(ws.begin(), ws.end());
      out.push_back({ep.first.second, std::move(ws)});
    }
  }
  std::sort(out.begin(), out.end(),
      [](const auto& a, const auto& b) {
        return a.first < b.first;
      });

  for (const auto& row : out) {
    std::cout << row.first;
    for (unsigned w : row.second) {
      std::cout << " " << w;
    }
    std::cout << "\n";
  }
}

void alberto::cmdInbound(const GraphTable& graphs,
                          const std::vector< std::string >& tok)
{
  if (tok.size() < 3 || !graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  const Graph& g = graphs.get(tok[1]);
  if (!g.hasVertex(tok[2])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const std::string& dst = tok[2];
  std::vector< std::pair< std::string, std::vector< unsigned > > > in;

  for (const auto& ep : g.edges) {
    if (ep.first.second == dst) {
      std::vector< unsigned > ws;
      for (unsigned w : ep.second) {
        ws.push_back(w);
      }
      std::sort(ws.begin(), ws.end());
      in.push_back({ep.first.first, std::move(ws)});
    }
  }
  std::sort(in.begin(), in.end(),
      [](const auto& a, const auto& b) {
        return a.first < b.first;
      });

  for (const auto& row : in) {
    std::cout << row.first;
    for (unsigned w : row.second) {
      std::cout << " " << w;
    }
    std::cout << "\n";
  }
}

void alberto::cmdBind(GraphTable& graphs,
                       const std::vector< std::string >& tok)
{
  if (tok.size() < 5 || !graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  const unsigned w = static_cast< unsigned >(std::stoul(tok[4]));
  Graph& g = graphs.get(tok[1]);
  try {
    g.addEdge(tok[2], tok[3], w);
  } catch (std::overflow_error&) {
    g.edges.rehash(g.edges.bucketCount() * 2);
    g.vertices.rehash(g.vertices.bucketCount() * 2);
    g.addEdge(tok[2], tok[3], w);
  }
}

void alberto::cmdCut(GraphTable& graphs,
                      const std::vector< std::string >& tok)
{
  if (tok.size() < 5 || !graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  Graph& g = graphs.get(tok[1]);
  if (!g.hasVertex(tok[2]) || !g.hasVertex(tok[3])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const unsigned w = static_cast< unsigned >(std::stoul(tok[4]));

  if (!g.cutEdge(tok[2], tok[3], w)) {
    std::cout << "<INVALID COMMAND>\n";
  }
}

void alberto::cmdCreate(GraphTable& graphs,
                         const std::vector< std::string >& tok)
{
  if (tok.size() < 2 || graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }
  Graph  ng;
  size_t cnt = 0;
  if (tok.size() >= 3) {
    cnt = static_cast< size_t >(std::stoul(tok[2]));
    for (size_t i = 0; i < cnt && (3 + i) < tok.size(); ++i) {
      ng.ensureVertex(tok[3 + i]);
    }
  }
  safeAddGraph(graphs, tok[1], std::move(ng));
}

void alberto::cmdMerge(GraphTable& graphs,
                        const std::vector< std::string >& tok)
{
  if (tok.size() < 4
      || !graphs.has(tok[2]) || !graphs.has(tok[3])
      || graphs.has(tok[1]))
  {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  Graph ng;
  for (int s = 2; s <= 3; ++s) {
    const Graph& src = graphs.get(tok[s]);
    for (const auto& vp : src.vertices) {
      ng.ensureVertex(vp.first);
    }
    for (const auto& ep : src.edges) {
      for (unsigned w : ep.second) {
        const EdgeKey k = ep.first;
        if (!ng.edges.has(k)) {
          WeightList wl;
          wl.push_back(w);
          try {
            ng.edges.add(k, std::move(wl));
          } catch (std::overflow_error&) {
            ng.edges.rehash(ng.edges.bucketCount() * 2);
            ng.edges.add(k, std::move(wl));
          }
        } else {
          ng.edges.get(k).push_back(w);
        }
      }
    }
  }
  safeAddGraph(graphs, tok[1], std::move(ng));
}

void alberto::cmdExtract(GraphTable& graphs,
                          const std::vector< std::string >& tok)
{
  if (tok.size() < 4 || !graphs.has(tok[2]) || graphs.has(tok[1])) {
    std::cout << "<INVALID COMMAND>\n";
    return;
  }

  const size_t cnt = static_cast< size_t >(std::stoul(tok[3]));
  std::vector< std::string > verts;
  for (size_t i = 0; i < cnt && (4 + i) < tok.size(); ++i) {
    verts.push_back(tok[4 + i]);
  }

  const Graph& src = graphs.get(tok[2]);
  for (const auto& v : verts) {
    if (!src.hasVertex(v)) {
      std::cout << "<INVALID COMMAND>\n";
      return;
    }
  }

  Graph ng;
  for (const auto& v : verts) {
    ng.ensureVertex(v);
  }

  const auto inSet = [&](const std::string& x) {
    return std::find(verts.begin(), verts.end(), x) != verts.end();
  };

  for (const auto& ep : src.edges) {
    if (inSet(ep.first.first) && inSet(ep.first.second)) {
      for (unsigned w : ep.second) {
        const EdgeKey k = ep.first;
        if (!ng.edges.has(k)) {
          WeightList wl;
          wl.push_back(w);
          try {
            ng.edges.add(k, std::move(wl));
          } catch (std::overflow_error&) {
            ng.edges.rehash(ng.edges.bucketCount() * 2);
            ng.edges.add(k, std::move(wl));
          }
        } else {
          ng.edges.get(k).push_back(w);
        }
      }
    }
  }
  safeAddGraph(graphs, tok[1], std::move(ng));
}

void alberto::buildCommands(HashTable< std::string, CmdFn, xx_hash >& cmds)
{
  cmds.add("graphs",
      [](GraphTable& g, const std::vector< std::string >&) {
        cmdGraphs(g);
      });
  cmds.add("vertexes",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdVertexes(g, t);
      });
  cmds.add("outbound",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdOutbound(g, t);
      });
  cmds.add("inbound",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdInbound(g, t);
      });
  cmds.add("bind",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdBind(g, t);
      });
  cmds.add("cut",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdCut(g, t);
      });
  cmds.add("create",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdCreate(g, t);
      });
  cmds.add("merge",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdMerge(g, t);
      });
  cmds.add("extract",
      [](GraphTable& g, const std::vector< std::string >& t) {
        cmdExtract(g, t);
      });
}
