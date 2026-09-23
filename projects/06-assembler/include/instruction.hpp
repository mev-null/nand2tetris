#pragma once

#include <string>

namespace hack::assembler {

enum class InstructionType {
  kAInstruction,
  kCInstruction,
  kLInstruction,
};

struct CInstruction {
  std::string dest;
  std::string comp;
  std::string jump;
};

}  // namespace hack::assembler
