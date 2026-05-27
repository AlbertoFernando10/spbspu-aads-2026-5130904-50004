#include "graph.hpp"
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

void alberto::Graph::ensureVertex(const std::string& v)
{
  if (!vertices.has(v)) {
    vertices.add(v, true);
  }
}

void alberto::Graph::addEdge(const std::string& src,
                              const std::string& dst,
                              unsigned w)
{
  ensureVertex(src);
  ensureVertex(dst);

  const EdgeKey k{src, dst};
  if (!edges.has(k)) {
    WeightList wl;
    wl.push_back(w);
    edges.add(k, std::move(wl));
  } else {
    edges.get(k).push_back(w);
  }
}

bool alberto::Graph::cutEdge(const std::string& src,
                              const std::string& dst,
                              unsigned w)
{
  const EdgeKey k{src, dst};
  if (!edges.has(k)) {
    return false;
  }

  WeightList& wl = edges.get(k);
  WeightList new_wl;
  bool found = false;
  for (const auto& weight : wl) {
    if (!found && weight == w) {
      found = true;
    } else {
      new_wl.push_back(weight);
    }
  }

  if (!found) {
    return false;
  }

  if (new_wl.empty()) {
    edges.drop(k);
  } else {
    wl = std::move(new_wl);
  }
  return true;
}

bool alberto::Graph::hasVertex(const std::string& v) const
{
  return vertices.has(v);
}

std::vector< std::string >
alberto::tokenize(const std::string& line)
{
  std::vector< std::string > tokens;
  size_t i = 0;
  while (i < line.size()) {
    while (i < line.size() && line[i] == ' ') {
      ++i;
    }
    size_t j = i;
    while (j < line.size() && line[j] != ' ') {
      ++j;
    }
    if (j > i) {
      tokens.push_back(line.substr(i, j - i));
    }
    i = j;
  }
  return tokens;
}

void alberto::loadGraphs(const std::string& filename, GraphTable& graphs)
{
  std::ifstream f(filename);
  if (!f.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  while (std::getline(f, line)) {
    if (line.empty()) {
      continue;
    }

    const auto tokens = tokenize(line);
    if (tokens.empty() || tokens.size() < 2) {
      continue;
    }

    const std::string& gname      = tokens[0];
    const int          edgeCount  = std::stoi(tokens[1]);

    if (!graphs.has(gname)) {
      try {
        graphs.add(gname, Graph{});
      } catch (std::overflow_error&) {
        graphs.rehash(graphs.bucketCount() * 2);
        graphs.add(gname, Graph{});
      }
    }

    for (int e = 0; e < edgeCount; ) {
      if (!std::getline(f, line)) {
        break;
      }
      if (line.empty()) {
        continue;
      }

      const auto et = tokenize(line);
      if (et.empty()) {
        continue;
      }
      if (et.size() < 3) {
        ++e;
        continue;
      }

      const unsigned w = static_cast< unsigned >(std::stoul(et[2]));
      try {
        graphs.get(gname).addEdge(et[0], et[1], w);
      } catch (std::overflow_error&) {
        Graph& g = graphs.get(gname);
        g.edges.rehash(g.edges.bucketCount() * 2);
        g.vertices.rehash(g.vertices.bucketCount() * 2);
        g.addEdge(et[0], et[1], w);
      }
      ++e;
    }
  }
}

void alberto::safeAddGraph(GraphTable& graphs,
                            const std::string& name,
                            Graph g)
{
  try {
    graphs.add(name, std::move(g));
  } catch (std::overflow_error&) {
    graphs.rehash(graphs.bucketCount() * 2);
    graphs.add(name, std::move(g));
  }
}
