#pragma once

#include <string>

enum class InstructionType {
  A_INSTRUCTION,
  C_INSTRUCTION,
  L_INSTRUCTION,
};

struct CInstruction {
  std::string dest;
  std::string comp;
  std::string jump;
};

std::string process_line(std::string line);
InstructionType instruction_type(const std::string& instruction);
std::string parse_symbol(const std::string& instruction);
CInstruction parse_c_instruction(const std::string& instruction);
