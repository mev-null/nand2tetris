#pragma once

#include <string>

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
