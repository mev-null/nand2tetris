#include "code.hpp"

#include <bitset>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "instruction.hpp"

std::string CompCode(const std::string& comp) {
  static const std::unordered_map<std::string, std::string> kTable = {
      {"0",   "0101010"},
      {"1",   "0111111"},
      {"-1",  "0111010"},
      {"D",   "0001100"},
      {"A",   "0110000"},
      {"!D",  "0001101"},
      {"!A",  "0110001"},
      {"-D",  "0001111"},
      {"-A",  "0110011"},
      {"D+1", "0011111"},
      {"A+1", "0110111"},
      {"D-1", "0001110"},
      {"A-1", "0110010"},
      {"D+A", "0000010"},
      {"D-A", "0010011"},
      {"A-D", "0000111"},
      {"D&A", "0000000"},
      {"D|A", "0010101"},

      {"M",   "1110000"},
      {"!M",  "1110001"},
      {"-M",  "1110011"},
      {"M+1", "1110111"},
      {"M-1", "1110010"},
      {"D+M", "1000010"},
      {"D-M", "1010011"},
      {"M-D", "1000111"},
      {"D&M", "1000000"},
      {"D|M", "1010101"}
  };

  auto it = kTable.find(comp);
  if (it == kTable.end()) {
    throw std::invalid_argument("invalid comp");
  }
  return it->second;
}

std::string DestCode(const std::string& dest) {
  static const std::unordered_map<std::string, std::string> kTable = {
      {"",    "000"},
      {"M",   "001"},
      {"D",   "010"},
      {"MD",  "011"},
      {"A",   "100"},
      {"AM",  "101"},
      {"AD",  "110"},
      {"AMD", "111"}
  };

  auto it = kTable.find(dest);
  if (it == kTable.end()) {
    throw std::invalid_argument("invalid dest");
  }
  return it->second;
}

std::string JumpCode(const std::string& jump) {
  static const std::unordered_map<std::string, std::string> kTable = {
      {"",    "000"},
      {"JGT", "001"},
      {"JEQ", "010"},
      {"JGE", "011"},
      {"JLT", "100"},
      {"JNE", "101"},
      {"JLE", "110"},
      {"JMP", "111"}
  };

  auto it = kTable.find(jump);
  if (it == kTable.end()) {
    throw std::invalid_argument("invalid jump");
  }
  return it->second;
}

std::string EncodeAInstruction(int value) {
  if (value < 0 || value > 32767) {
    throw std::invalid_argument("A instruction value out of range");
  }

  std::string result = "0";
  result += std::bitset<15>(value).to_string();
  return result;
}

std::string EncodeCInstruction(const CInstruction& instruction) {
  std::string result = "111";
  result += CompCode(instruction.comp);
  result += DestCode(instruction.dest);
  result += JumpCode(instruction.jump);

  return result;
}
