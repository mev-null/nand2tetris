#pragma once

#include <string>

#include "instruction.hpp"

namespace hack::assembler {

std::string CompCode(const std::string& comp);
std::string DestCode(const std::string& dest);
std::string JumpCode(const std::string& jump);

std::string EncodeAInstruction(int value);
std::string EncodeCInstruction(const CInstruction& instruction);

}  // namespace hack::assembler
