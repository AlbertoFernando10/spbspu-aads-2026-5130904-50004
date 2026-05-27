#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char * argv[]);
#endif

#define BOOST_TEST_MODULE S3
#include <boost/test/included/unit_test.hpp>
#include "graph.hpp"
#include "commands.hpp"
#include <sstream>

BOOST_AUTO_TEST_CASE(main_test)
{
  alberto::Graph graph;
  graph.ensureVertex("a");
  graph.ensureVertex("b");
  graph.addEdge("a", "b", 76);
  BOOST_TEST(graph.hasVertex("a") == true);
  BOOST_TEST(graph.hasVertex("b") == true);
  const alberto::EdgeKey k{"a", "b"};
  BOOST_TEST(graph.edges.has(k) == true);
  alberto::Graph g2;
  g2.ensureVertex("d");
  g2.ensureVertex("a");
  g2.addEdge("d", "a", 0);
  BOOST_TEST(g2.edges.has({"d", "a"}) == true);
  const auto& wl = g2.edges.get({"d", "a"});
  bool hasZero = false;
  for (auto w : wl) {
    if (w == 0) hasZero = true;
  }
  BOOST_TEST(hasZero == true);
  g2.cutEdge("d", "a", 0);
  BOOST_TEST(g2.edges.has({"d", "a"}) == false);
  alberto::HashTable< std::string, int > ht;
  ht.add("test", 100);
  BOOST_TEST(ht.has("test") == true);
  BOOST_TEST(ht.get("test") == 100);
  int val = ht.drop("test");
  BOOST_TEST(val == 100);
  BOOST_TEST(ht.has("test") == false);
}
