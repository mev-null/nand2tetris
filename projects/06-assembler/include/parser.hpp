#pragma once

#include <string>

#include "instruction.hpp"

std::string process_line(std::string line);
InstructionType instruction_type(const std::string& instruction);
std::string parse_symbol(const std::string& instruction);
CInstruction parse_c_instruction(const std::string& instruction);
