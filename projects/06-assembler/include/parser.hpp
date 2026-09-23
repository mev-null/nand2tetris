#pragma once

#include <string>

#include "instruction.hpp"

std::string ProcessLine(std::string line);
InstructionType ClassifyInstruction(const std::string& instruction);
std::string ParseSymbol(const std::string& instruction);
CInstruction ParseCInstruction(const std::string& instruction);
