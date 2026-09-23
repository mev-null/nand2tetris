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
