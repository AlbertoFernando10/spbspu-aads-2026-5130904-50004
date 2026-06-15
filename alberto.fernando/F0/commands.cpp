#include "commands.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace alberto {

std::vector<std::string> tokenize(const std::string& line)
{
  std::vector<std::string> tokens;
  std::istringstream iss(line);
  std::string tok;
  while (iss >> tok) {
    tokens.push_back(tok);
  }
  return tokens;
}

void cmdLocal(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 3) {
    throw std::invalid_argument("local: wrong number of arguments");
  }
  const std::string& name = tok[1];
  const std::string& filename = tok[2];
  if (s.texts.has(name)) {
    throw std::invalid_argument("local: name already in use");
  }
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::invalid_argument("local: cannot open file");
  }
  std::string content((std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>());
  if (content.empty()) {
    throw std::invalid_argument("local: file is empty");
  }
  s.texts.add(name, TextEntry(content, TextState::RAW, filename));
  std::cout << "<TEXT LOADED: " << name << ">\n";
}

void cmdShowText(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 2) {
    throw std::invalid_argument("show-text: wrong number of arguments");
  }
  const std::string& name = tok[1];
  if (!s.texts.has(name)) {
    throw std::invalid_argument("show-text: text not found");
  }
  const TextEntry& entry = s.texts.get(name);
  if (entry.state_ == TextState::RAW) {
    std::cout << "<TEXT: " << name
              << ", STATE: raw"
              << ", FILE: " << entry.sourceFile_ << ">\n";
  } else {
    std::cout << "<TEXT: " << name
              << ", STATE: encoded"
              << ", CODING: " << entry.codingName_ << ">\n";
  }
}

void cmdDropText(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 2) {
    throw std::invalid_argument("drop-text: wrong number of arguments");
  }
  const std::string& name = tok[1];
  if (!s.texts.has(name)) {
    throw std::invalid_argument("drop-text: text not found");
  }
  s.texts.drop(name);
  std::cout << "<TEXT DROPPED: " << name << ">\n";
}

void cmdListTexts(Session& s, const std::vector<std::string>& tok)
{
  (void)tok;
  if (s.texts.empty()) {
    throw std::invalid_argument("list-texts: no texts loaded");
  }
  std::cout << "<TEXTS: " << s.texts.size() << ">\n";
  for (const auto& p : s.texts) {
    const TextEntry& e = p.second;
    if (e.state_ == TextState::RAW) {
      std::cout << p.first
                << " | STATE: raw"
                << " | FILE: " << e.sourceFile_ << "\n";
    } else {
      std::cout << p.first
                << " | STATE: encoded"
                << " | CODING: " << e.codingName_ << "\n";
    }
  }
}

void cmdBuildCoding(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 3) {
    throw std::invalid_argument("build-coding: wrong number of arguments");
  }
  const std::string& codingName = tok[1];
  const std::string& textName = tok[2];
  if (s.codings.has(codingName)) {
    throw std::invalid_argument("build-coding: coding name already in use");
  }
  if (!s.texts.has(textName)) {
    throw std::invalid_argument("build-coding: text not found");
  }
  const TextEntry& entry = s.texts.get(textName);
  if (entry.state_ == TextState::ENCODED) {
    throw std::invalid_argument("build-coding: text is already encoded");
  }
  const std::string& content = entry.content_;
  const size_t total = content.size();
  auto freq = buildFreqTable(content);
  HuffNode* const root = buildTree(freq);
  CodingEntry* const coding = new CodingEntry();
  coding->sourceName_ = textName;
  coding->root_ = root;
  buildCodesHelper(root, "", coding->codes_);
  coding->uniqueChars_ = freq.size();
  coding->originalBits_ = total * 8;
  coding->entropy_ = calcEntropy(freq, total);
  size_t compressed = 0;
  for (const auto& p : freq) {
    compressed += p.second * coding->codes_.get(p.first).size();
  }
  coding->compressedBits_ = compressed;
  s.codings.add(codingName, coding);
  std::cout << "<CODING BUILT: " << codingName
            << ", UNIQUE CHARS: " << coding->uniqueChars_ << ">\n";
}

void cmdShowCodes(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 2) {
    throw std::invalid_argument("show-codes: wrong number of arguments");
  }
  const std::string& codingName = tok[1];
  if (!s.codings.has(codingName)) {
    throw std::invalid_argument("show-codes: coding not found");
  }
  const CodingEntry* const coding = s.codings.get(codingName);
  std::cout << "<HUFFMAN CODES: " << codingName << ">\n";
  for (const auto& p : coding->codes_) {
    std::cout << "'" << p.first << "': code=" << p.second << "\n";
  }
}

void cmdDropCoding(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 2) {
    throw std::invalid_argument("drop-coding: wrong number of arguments");
  }
  const std::string& codingName = tok[1];
  if (!s.codings.has(codingName)) {
    throw std::invalid_argument("drop-coding: coding not found");
  }
  CodingEntry* const ptr = s.codings.drop(codingName);
  delete ptr;
  std::cout << "<CODING DROPPED: " << codingName << ">\n";
}

void cmdListCodings(Session& s, const std::vector<std::string>& tok)
{
  (void)tok;
  if (s.codings.empty()) {
    throw std::invalid_argument("list-codings: no codings built");
  }
  std::cout << "<CODINGS: " << s.codings.size() << ">\n";
  for (const auto& p : s.codings) {
    const CodingEntry* const c = p.second;
    std::cout << p.first
              << " | SOURCE: " << c->sourceName_
              << " | UNIQUE CHARS: " << c->uniqueChars_ << "\n";
  }
}

void cmdEncode(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 4) {
    throw std::invalid_argument("encode: wrong number of arguments");
  }
  const std::string& srcName = tok[1];
  const std::string& dstName = tok[2];
  const std::string& codingName = tok[3];
  if (!s.texts.has(srcName)) {
    throw std::invalid_argument("encode: source text not found");
  }
  if (s.texts.has(dstName)) {
    throw std::invalid_argument("encode: result name already in use");
  }
  if (!s.codings.has(codingName)) {
    throw std::invalid_argument("encode: coding not found");
  }
  const TextEntry& src = s.texts.get(srcName);
  if (src.state_ == TextState::ENCODED) {
    throw std::invalid_argument("encode: source text is already encoded");
  }
  const CodingEntry* const coding = s.codings.get(codingName);
  std::string bits;
  for (char c : src.content_) {
    if (!coding->codes_.has(c)) {
      throw std::invalid_argument("encode: character not in coding");
    }
    bits += coding->codes_.get(c);
  }
  s.texts.add(dstName, TextEntry(bits, TextState::ENCODED, "", codingName));
  std::cout << "<ENCODED: " << srcName << " -> " << dstName
            << " USING " << codingName << ">\n";
}

static std::string decodeWithTree(const std::string& bits, HuffNode* const root)
{
  if (!root) {
    throw std::invalid_argument("decode: empty coding tree");
  }
  if (!root->left_ && !root->right_) {
    std::string result;
    for (char b : bits) {
      if (b != '0') {
        throw std::invalid_argument("decode: invalid bit sequence");
      }
      result += root->ch_;
    }
    return result;
  }
  std::string result;
  HuffNode* cur = root;
  for (char b : bits) {
    if (b == '0') {
      cur = cur->left_;
    } else if (b == '1') {
      cur = cur->right_;
    } else {
      throw std::invalid_argument("decode: invalid bit sequence");
    }
    if (!cur) {
      throw std::invalid_argument("decode: invalid bit sequence");
    }
    if (!cur->left_ && !cur->right_) {
      result += cur->ch_;
      cur = root;
    }
  }
  if (cur != root) {
    throw std::invalid_argument("decode: incomplete bit sequence");
  }
  return result;
}

void cmdDecode(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 4) {
    throw std::invalid_argument("decode: wrong number of arguments");
  }
  const std::string& srcName = tok[1];
  const std::string& dstName = tok[2];
  const std::string& codingName = tok[3];
  if (!s.texts.has(srcName)) {
    throw std::invalid_argument("decode: source text not found");
  }
  if (s.texts.has(dstName)) {
    throw std::invalid_argument("decode: result name already in use");
  }
  if (!s.codings.has(codingName)) {
    throw std::invalid_argument("decode: coding not found");
  }
  const TextEntry& src = s.texts.get(srcName);
  if (src.state_ != TextState::ENCODED) {
    throw std::invalid_argument("decode: source text is not encoded");
  }
  const CodingEntry* const coding = s.codings.get(codingName);
  const std::string decoded = decodeWithTree(src.content_, coding->root_);
  s.texts.add(dstName, TextEntry(decoded, TextState::RAW, "", ""));
  std::cout << "<DECODED: " << srcName << " -> " << dstName
            << " USING " << codingName << ">\n";
}

void cmdSave(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 3) {
    throw std::invalid_argument("save: wrong number of arguments");
  }
  const std::string& name = tok[1];
  const std::string& filename = tok[2];
  if (!s.texts.has(name)) {
    throw std::invalid_argument("save: text not found");
  }
  std::ofstream file(filename);
  if (!file.is_open()) {
    throw std::invalid_argument("save: cannot open file for writing");
  }
  file << s.texts.get(name).content_;
  std::cout << "<SAVED: " << name << " -> " << filename << ">\n";
}

void cmdAnalyze(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 2) {
    throw std::invalid_argument("analyze: wrong number of arguments");
  }
  const std::string& codingName = tok[1];
  if (!s.codings.has(codingName)) {
    throw std::invalid_argument("analyze: coding not found");
  }
  const CodingEntry* const c = s.codings.get(codingName);
  const double ratio = c->originalBits_ > 0
      ? 100.0 * (1.0 - static_cast<double>(c->compressedBits_)
          / c->originalBits_)
      : 0.0;
  std::cout << "<COMPRESSION ANALYSIS: " << codingName << ">\n"
            << "ORIGINAL SIZE:   " << c->originalBits_ << " bits\n"
            << "COMPRESSED SIZE: " << c->compressedBits_ << " bits\n"
            << std::fixed << std::setprecision(2)
            << "RATIO:           " << ratio << "% reduction\n"
            << "ENTROPY:         " << c->entropy_ << " bits/char\n";
}

void cmdCompare(Session& s, const std::vector<std::string>& tok)
{
  if (tok.size() != 3) {
    throw std::invalid_argument("compare: wrong number of arguments");
  }
  const std::string& name1 = tok[1];
  const std::string& name2 = tok[2];
  if (!s.codings.has(name1)) {
    throw std::invalid_argument("compare: first coding not found");
  }
  if (!s.codings.has(name2)) {
    throw std::invalid_argument("compare: second coding not found");
  }
  const CodingEntry* const c1 = s.codings.get(name1);
  const CodingEntry* const c2 = s.codings.get(name2);
  const double r1 = c1->originalBits_ > 0
      ? 100.0 * (1.0 - static_cast<double>(c1->compressedBits_)
          / c1->originalBits_)
      : 0.0;
  const double r2 = c2->originalBits_ > 0
      ? 100.0 * (1.0 - static_cast<double>(c2->compressedBits_)
          / c2->originalBits_)
      : 0.0;
  std::cout << "<COMPRESSION COMPARISON>\n"
            << std::fixed << std::setprecision(2)
            << name1
            << " | ORIGINAL: " << c1->originalBits_ << " bits"
            << " | COMPRESSED: " << c1->compressedBits_ << " bits"
            << " | RATIO: " << r1 << "% reduction\n"
            << name2
            << " | ORIGINAL: " << c2->originalBits_ << " bits"
            << " | COMPRESSED: " << c2->compressedBits_ << " bits"
            << " | RATIO: " << r2 << "% reduction\n";
}

void buildCommands(HashTable<std::string, CmdFn, xx_hash>& cmds)
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
