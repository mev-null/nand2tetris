#include "assembler.hpp"

#include <bitset>
#include <stdexcept>
#include <string>

#include "code.hpp"
#include "parser.hpp"

std::string encode_a_instruction(const std::string& instruction) {
  std::string symbol = parse_symbol(instruction);

  int value = std::stoi(symbol);

  if (value < 0 || value > 32767) {
    throw std::invalid_argument("A instruction value out of range");
  }

  std::string result = "0";
  result += std::bitset<15>(value).to_string();
  return result;
}

std::string encode_c_instruction(const std::string& instruction) {
  CInstruction parsed = parse_c_instruction(instruction);
  std::string result = "111";
  result += comp_code(parsed.comp);
  result += dest_code(parsed.dest);
  result += jump_code(parsed.jump);

  return result;
}
