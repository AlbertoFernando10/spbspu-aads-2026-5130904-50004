#if defined(__GNUC__) || defined(__clang__)
__attribute__((weak)) int main(int argc, char * argv[]);
#endif

#define BOOST_TEST_MODULE HuffmanTests
#include <boost/test/included/unit_test.hpp>

#include <string>
#include <vector>
#include <sstream>

#include "commands.hpp"
#include "huffman.hpp"

BOOST_AUTO_TEST_CASE(testHashTableAddAndGet)
{
  using namespace alberto;

  HashTable< std::string, int > table;

  BOOST_CHECK(table.empty());
  BOOST_CHECK_EQUAL(table.size(), 0);

  table.add("key1", 42);
  BOOST_CHECK(!table.empty());
  BOOST_CHECK_EQUAL(table.size(), 1);

  BOOST_REQUIRE(table.has("key1"));
  BOOST_CHECK_EQUAL(table.get("key1"), 42);

  table.add("key2", 100);
  BOOST_CHECK_EQUAL(table.size(), 2);

  BOOST_REQUIRE(table.has("key2"));
  BOOST_CHECK_EQUAL(table.get("key2"), 100);
}

BOOST_AUTO_TEST_CASE(testHashTableDrop)
{
  using namespace alberto;

  HashTable< std::string, int > table;

  table.add("a", 1);
  table.add("b", 2);
  table.add("c", 3);

  BOOST_CHECK_EQUAL(table.size(), 3);

  BOOST_REQUIRE(table.has("a"));
  BOOST_REQUIRE(table.has("b"));
  BOOST_REQUIRE(table.has("c"));

  BOOST_CHECK_EQUAL(table.get("a"), 1);
  BOOST_CHECK_EQUAL(table.get("b"), 2);
  BOOST_CHECK_EQUAL(table.get("c"), 3);

  int dropped = table.drop("b");
  BOOST_CHECK_EQUAL(dropped, 2);
  BOOST_CHECK_EQUAL(table.size(), 2);

  BOOST_CHECK(!table.has("b"));
  BOOST_CHECK(table.has("a"));
  BOOST_CHECK(table.has("c"));
}

BOOST_AUTO_TEST_CASE(testHashTableIterator)
{
  using namespace alberto;

  HashTable< std::string, int > table;

  table.add("a", 1);
  table.add("b", 2);
  table.add("c", 3);

  size_t count = 0;
  for (const auto & p : table) {
    BOOST_CHECK(p.second >= 1 && p.second <= 3);
    ++count;
  }
  BOOST_CHECK_EQUAL(count, 3);
}

BOOST_AUTO_TEST_CASE(testBuildFreqTable)
{
  using namespace alberto;

  std::string text = "aabbbcccc";
  auto freq = buildFreqTable(text);

  BOOST_REQUIRE(freq.has('a'));
  BOOST_CHECK_EQUAL(freq.get('a'), 2);

  BOOST_REQUIRE(freq.has('b'));
  BOOST_CHECK_EQUAL(freq.get('b'), 3);

  BOOST_REQUIRE(freq.has('c'));
  BOOST_CHECK_EQUAL(freq.get('c'), 4);
}

BOOST_AUTO_TEST_CASE(testBuildTreeAndCodes)
{
  using namespace alberto;

  std::string text = "abcd";
  auto freq = buildFreqTable(text);
  auto root = buildTree(freq);

  BOOST_REQUIRE(root != nullptr);

  HashTable< char, std::string, xx_hash > codes(CODING_HASH_CAP);
  buildCodesHelper(root.get(), "", codes);

  BOOST_REQUIRE(codes.has('a'));
  BOOST_REQUIRE(codes.has('b'));
  BOOST_REQUIRE(codes.has('c'));
  BOOST_REQUIRE(codes.has('d'));

  BOOST_CHECK_EQUAL(codes.get('a').size(), 2);
  BOOST_CHECK_EQUAL(codes.get('b').size(), 2);
  BOOST_CHECK_EQUAL(codes.get('c').size(), 2);
  BOOST_CHECK_EQUAL(codes.get('d').size(), 2);

  BOOST_CHECK(codes.get('a') != codes.get('b'));
  BOOST_CHECK(codes.get('a') != codes.get('c'));
  BOOST_CHECK(codes.get('b') != codes.get('c'));
}

BOOST_AUTO_TEST_CASE(testCalcEntropy)
{
  using namespace alberto;

  std::string text = "aabb";
  auto freq = buildFreqTable(text);
  double entropy = calcEntropy(freq, text.size());

  BOOST_CHECK(entropy > 0.99 && entropy < 1.01);
}

BOOST_AUTO_TEST_CASE(testEncodeDecode)
{
  using namespace alberto;

  std::string text = "abracadabra";

  auto freq = buildFreqTable(text);
  auto root = buildTree(freq);

  BOOST_REQUIRE(root != nullptr);

  HashTable< char, std::string, xx_hash > codes(CODING_HASH_CAP);
  buildCodesHelper(root.get(), "", codes);

  for (char c : text) {
    BOOST_REQUIRE(codes.has(c));
  }

  CodingEntry coding;
  coding.m_codes = std::move(codes);
  coding.m_root = std::move(root);

  std::string encoded = encodeToBits(text, &coding);
  BOOST_REQUIRE(!encoded.empty());

  std::string decoded = decodeFromBits(encoded, coding.m_root.get());
  BOOST_CHECK(decoded == text);
}

BOOST_AUTO_TEST_CASE(testSessionAndCommands)
{
  using namespace alberto;

  Session session;

  std::string content = "hello world";
  session.m_texts.add("test", TextEntry(content, TextState::RAW, "fake_file.txt"));

  BOOST_REQUIRE(session.m_texts.has("test"));
  BOOST_CHECK(session.m_texts.get("test").m_content == content);
  BOOST_CHECK(session.m_texts.get("test").m_state == TextState::RAW);

  auto freq = buildFreqTable(content);
  auto root = buildTree(freq);

  BOOST_REQUIRE(root != nullptr);

  auto coding = std::make_unique< CodingEntry >();
  coding->m_root = std::move(root);
  coding->m_sourceName = "test";
  buildCodesHelper(coding->m_root.get(), "", coding->m_codes);
  coding->m_uniqueChars = freq.size();
  coding->m_originalBits = content.size() * 8;

  size_t compressed = 0;
  for (const auto & p : freq) {
    compressed += p.second * coding->m_codes.get(p.first).size();
  }
  coding->m_compressedBits = compressed;
  coding->m_entropy = calcEntropy(freq, content.size());

  session.m_codings.add("huff1", std::move(coding));

  BOOST_REQUIRE(session.m_codings.has("huff1"));

  const TextEntry & src = session.m_texts.get("test");
  const CodingEntry * c = session.m_codings.get("huff1").get();

  BOOST_REQUIRE(c != nullptr);

  std::string encoded = encodeToBits(src.m_content, c);
  BOOST_REQUIRE(!encoded.empty());

  session.m_texts.add("encoded",
                       TextEntry(encoded, TextState::ENCODED, "", "huff1"));

  BOOST_REQUIRE(session.m_texts.has("encoded"));
  BOOST_CHECK(session.m_texts.get("encoded").m_state == TextState::ENCODED);

  const TextEntry & srcEnc = session.m_texts.get("encoded");
  std::string decoded = decodeFromBits(srcEnc.m_content, c->m_root.get());
  session.m_texts.add("decoded",
                       TextEntry(decoded, TextState::RAW, "", ""));

  BOOST_REQUIRE(session.m_texts.has("decoded"));
  BOOST_CHECK(session.m_texts.get("decoded").m_content == content);
  BOOST_CHECK(session.m_texts.get("decoded").m_state == TextState::RAW);
}

BOOST_AUTO_TEST_CASE(testEmptyText)
{
  using namespace alberto;

  std::string text = "";
  auto freq = buildFreqTable(text);

  BOOST_CHECK(freq.empty());
  BOOST_CHECK_EQUAL(freq.size(), 0);

  auto root = buildTree(freq);
  BOOST_CHECK(root == nullptr);
}

BOOST_AUTO_TEST_CASE(testSingleChar)
{
  using namespace alberto;

  std::string text = "aaaaa";
  auto freq = buildFreqTable(text);

  BOOST_CHECK_EQUAL(freq.size(), 1);

  BOOST_REQUIRE(freq.has('a'));
  BOOST_CHECK_EQUAL(freq.get('a'), 5);

  auto root = buildTree(freq);
  BOOST_REQUIRE(root != nullptr);

  HashTable< char, std::string, xx_hash > codes(CODING_HASH_CAP);
  buildCodesHelper(root.get(), "", codes);

  BOOST_REQUIRE(codes.has('a'));
  BOOST_CHECK(codes.get('a') == "0");
}
