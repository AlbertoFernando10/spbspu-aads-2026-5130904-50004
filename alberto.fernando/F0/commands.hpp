#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include "huffman.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace alberto {

constexpr size_t SESSION_TABLE_CAP = 32;

struct Session
{
  TextTable m_texts;
  CodingTable m_codings;

  Session() :
    m_texts(SESSION_TABLE_CAP),
    m_codings(SESSION_TABLE_CAP)
  {}
  ~Session() = default;
};

using CmdFn = std::function< void(Session &, const std::vector< std::string > &) >;

void cmdLocal(Session & s, const std::vector< std::string > & tok);
void cmdShowText(Session & s, const std::vector< std::string > & tok);
void cmdDropText(Session & s, const std::vector< std::string > & tok);
void cmdListTexts(Session & s, const std::vector< std::string > & tok);
void cmdBuildCoding(Session & s, const std::vector< std::string > & tok);
void cmdShowCodes(Session & s, const std::vector< std::string > & tok);
void cmdDropCoding(Session & s, const std::vector< std::string > & tok);
void cmdListCodings(Session & s, const std::vector< std::string > & tok);
void cmdEncode(Session & s, const std::vector< std::string > & tok);
void cmdDecode(Session & s, const std::vector< std::string > & tok);
void cmdSave(Session & s, const std::vector< std::string > & tok);
void cmdAnalyze(Session & s, const std::vector< std::string > & tok);
void cmdCompare(Session & s, const std::vector< std::string > & tok);

void buildCommands(HashTable< std::string, CmdFn, xx_hash > & cmds);

}

#endif
