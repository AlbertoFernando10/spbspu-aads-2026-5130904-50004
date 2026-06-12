#include "commands.hpp"
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::vector< std::string > alberto::tokenize(const std::string& line)
{
  std::vector< std::string > tokens;
  std::istringstream iss(line);
  std::string tok;
  while (iss >> tok) {
    tokens.push_back(tok);
  }
  return tokens;
}
void alberto::cmdLocal(Session& s, const std::vector< std::string >& tok)
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
  std::string content((std::istreambuf_iterator< char >(file)),
      std::istreambuf_iterator< char >());
  if (content.empty()) {
    throw std::invalid_argument("local: file is empty");
  }
  s.texts.add(name, TextEntry(content, TextState::RAW, filename));
  std::cout << "<TEXT LOADED: " << name << ">\n";
}

void alberto::cmdShowText(Session& s, const std::vector< std::string >& tok)
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
void alberto::cmdDropText(Session& s, const std::vector< std::string >& tok)
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

void alberto::cmdListTexts(Session& s, const std::vector< std::string >& tok)
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

void alberto::cmdBuildCoding(Session& s, const std::vector< std::string >& tok)
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
