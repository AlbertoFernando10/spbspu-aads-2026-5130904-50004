#include "commands.hpp"
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace alberto {

constexpr size_t ARG_LOCAL = 3;
constexpr size_t ARG_SHOW_TEXT = 2;
constexpr size_t ARG_DROP_TEXT = 2;
constexpr size_t ARG_BUILD_CODING = 3;
constexpr size_t ARG_SHOW_CODES = 2;
constexpr size_t ARG_DROP_CODING = 2;
constexpr size_t ARG_ENCODE = 4;
constexpr size_t ARG_DECODE = 4;
constexpr size_t ARG_SAVE = 3;
constexpr size_t ARG_ANALYZE = 2;
constexpr size_t ARG_COMPARE = 3;

void cmdLocal(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_LOCAL)
    throw std::invalid_argument("local: wrong number of arguments");

  const std::string & name = tok[1];
  const std::string & filename = tok[2];

  if (s.m_texts.has(name))
    throw std::invalid_argument("local: name already in use");

  std::ifstream file(filename);
  if (!file.is_open())
    throw std::invalid_argument("local: cannot open file");

  std::string content((std::istreambuf_iterator< char >(file)),
                      std::istreambuf_iterator< char >());

  if (content.empty())
    throw std::invalid_argument("local: file is empty");

  s.m_texts.add(name, TextEntry(content, TextState::RAW, filename));
  std::cout << "<TEXT LOADED: " << name << ">\n";
}

void cmdShowText(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_SHOW_TEXT)
    throw std::invalid_argument("show-text: wrong number of arguments");

  const std::string & name = tok[1];
  if (!s.m_texts.has(name))
    throw std::invalid_argument("show-text: text not found");

  const TextEntry & entry = s.m_texts.get(name);
  std::cout << "<" << entry.getInfo() << ">\n";
}

void cmdDropText(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_DROP_TEXT)
    throw std::invalid_argument("drop-text: wrong number of arguments");

  const std::string & name = tok[1];
  if (!s.m_texts.has(name))
    throw std::invalid_argument("drop-text: text not found");

  s.m_texts.drop(name);
  std::cout << "<TEXT DROPPED: " << name << ">\n";
}

void cmdListTexts(Session & s, const std::vector< std::string > & tok)
{
  (void)tok;
  if (s.m_texts.empty())
    throw std::invalid_argument("list-texts: no texts loaded");

  std::cout << "<TEXTS: " << s.m_texts.size() << ">\n";
  for (const auto & p : s.m_texts) {
    std::cout << p.first << " | " << p.second.getInfo() << "\n";
  }
}

void cmdBuildCoding(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_BUILD_CODING)
    throw std::invalid_argument("build-coding: wrong number of arguments");

  const std::string & codingName = tok[1];
  const std::string & textName = tok[2];

  if (s.m_codings.has(codingName))
    throw std::invalid_argument("build-coding: coding name already in use");
  if (!s.m_texts.has(textName))
    throw std::invalid_argument("build-coding: text not found");

  const TextEntry & entry = s.m_texts.get(textName);
  if (entry.getState() == TextState::ENCODED)
    throw std::invalid_argument("build-coding: text is already encoded");

  const std::string & content = entry.getContent();
  const size_t total = content.size();

  auto freq = buildFreqTable(content);
  auto root = buildTree(freq);

  auto coding = std::make_unique< CodingEntry >();
  coding->m_sourceName = textName;
  coding->m_root = std::move(root);
  buildCodesHelper(coding->m_root.get(), "", coding->m_codes);
  coding->m_uniqueChars = freq.size();
  coding->m_originalBits = total * 8;
  coding->m_entropy = calcEntropy(freq, total);

  size_t compressed = 0;
  for (const auto & p : freq) {
    compressed += p.second * coding->m_codes.get(p.first).size();
  }
  coding->m_compressedBits = compressed;

  const size_t uniqueChars = coding->m_uniqueChars;
  s.m_codings.add(codingName, std::move(coding));

  std::cout << "<CODING BUILT: " << codingName
            << ", UNIQUE CHARS: " << uniqueChars << ">\n";
}

void cmdShowCodes(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_SHOW_CODES)
    throw std::invalid_argument("show-codes: wrong number of arguments");

  const std::string & codingName = tok[1];
  if (!s.m_codings.has(codingName))
    throw std::invalid_argument("show-codes: coding not found");

  const CodingEntry & coding = *s.m_codings.get(codingName);
  std::cout << "<" << coding.getCodesInfo() << ">\n";
}

void cmdDropCoding(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_DROP_CODING)
    throw std::invalid_argument("drop-coding: wrong number of arguments");

  const std::string & codingName = tok[1];
  if (!s.m_codings.has(codingName))
    throw std::invalid_argument("drop-coding: coding not found");

  s.m_codings.drop(codingName);
  std::cout << "<CODING DROPPED: " << codingName << ">\n";
}

void cmdListCodings(Session & s, const std::vector< std::string > & tok)
{
  (void)tok;
  if (s.m_codings.empty())
    throw std::invalid_argument("list-codings: no codings built");

  std::cout << "<CODINGS: " << s.m_codings.size() << ">\n";
  for (const auto & p : s.m_codings) {
    const CodingEntry & coding = *p.second;
    std::cout << p.first
              << " | SOURCE: " << coding.getSourceName()
              << " | UNIQUE CHARS: " << coding.getUniqueChars() << "\n";
  }
}

void cmdEncode(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_ENCODE)
    throw std::invalid_argument("encode: wrong number of arguments");

  const std::string & srcName = tok[1];
  const std::string & dstName = tok[2];
  const std::string & codingName = tok[3];

  if (!s.m_texts.has(srcName))
    throw std::invalid_argument("encode: source text not found");
  if (s.m_texts.has(dstName))
    throw std::invalid_argument("encode: result name already in use");
  if (!s.m_codings.has(codingName))
    throw std::invalid_argument("encode: coding not found");

  const TextEntry & src = s.m_texts.get(srcName);
  if (src.getState() == TextState::ENCODED)
    throw std::invalid_argument("encode: source text is already encoded");

  const CodingEntry & coding = *s.m_codings.get(codingName);
  const std::string encoded = encodeToBits(src.getContent(), &coding);
  s.m_texts.add(dstName, TextEntry(encoded, TextState::ENCODED, "", codingName));

  std::cout << "<ENCODED: " << srcName << " -> " << dstName
            << " USING " << codingName << ">\n";
}

void cmdDecode(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_DECODE)
    throw std::invalid_argument("decode: wrong number of arguments");

  const std::string & srcName = tok[1];
  const std::string & dstName = tok[2];
  const std::string & codingName = tok[3];

  if (!s.m_texts.has(srcName))
    throw std::invalid_argument("decode: source text not found");
  if (s.m_texts.has(dstName))
    throw std::invalid_argument("decode: result name already in use");
  if (!s.m_codings.has(codingName))
    throw std::invalid_argument("decode: coding not found");

  const TextEntry & src = s.m_texts.get(srcName);
  if (src.getState() != TextState::ENCODED)
    throw std::invalid_argument("decode: source text is not encoded");

  const CodingEntry & coding = *s.m_codings.get(codingName);
  const std::string decoded = decodeFromBits(src.getContent(), coding.getRoot());
  s.m_texts.add(dstName, TextEntry(decoded, TextState::RAW, "", ""));

  std::cout << "<DECODED: " << srcName << " -> " << dstName
            << " USING " << codingName << ">\n";
}

void cmdSave(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_SAVE)
    throw std::invalid_argument("save: wrong number of arguments");

  const std::string & name = tok[1];
  const std::string & filename = tok[2];

  if (!s.m_texts.has(name))
    throw std::invalid_argument("save: text not found");

  std::ofstream file(filename);
  if (!file.is_open())
    throw std::invalid_argument("save: cannot open file for writing");

  file << s.m_texts.get(name).getContent();
  std::cout << "<SAVED: " << name << " -> " << filename << ">\n";
}

void cmdAnalyze(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_ANALYZE)
    throw std::invalid_argument("analyze: wrong number of arguments");

  const std::string & codingName = tok[1];
  if (!s.m_codings.has(codingName))
    throw std::invalid_argument("analyze: coding not found");

  const CodingEntry & coding = *s.m_codings.get(codingName);
  std::cout << "<" << coding.getAnalysisInfo() << ">\n";
}

void cmdCompare(Session & s, const std::vector< std::string > & tok)
{
  if (tok.size() != ARG_COMPARE)
    throw std::invalid_argument("compare: wrong number of arguments");

  const std::string & name1 = tok[1];
  const std::string & name2 = tok[2];

  if (!s.m_codings.has(name1))
    throw std::invalid_argument("compare: first coding not found");
  if (!s.m_codings.has(name2))
    throw std::invalid_argument("compare: second coding not found");

  const CodingEntry & c1 = *s.m_codings.get(name1);
  const CodingEntry & c2 = *s.m_codings.get(name2);

  std::cout << "<" << c1.getComparisonInfo(c2) << ">\n";
}

void buildCommands(HashTable< std::string, CmdFn, xx_hash > & cmds)
{
  cmds.add("local", cmdLocal);
  cmds.add("show-text", cmdShowText);
  cmds.add("drop-text", cmdDropText);
  cmds.add("list-texts", cmdListTexts);
  cmds.add("build-coding", cmdBuildCoding);
  cmds.add("show-codes", cmdShowCodes);
  cmds.add("drop-coding", cmdDropCoding);
  cmds.add("list-codings", cmdListCodings);
  cmds.add("encode", cmdEncode);
  cmds.add("decode", cmdDecode);
  cmds.add("save", cmdSave);
  cmds.add("analyze", cmdAnalyze);
  cmds.add("compare", cmdCompare);
}

}
