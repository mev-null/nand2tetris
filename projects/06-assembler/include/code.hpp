#pragma once

#include <string>

#include "instruction.hpp"

std::string comp_code(const std::string& comp);
std::string dest_code(const std::string& dest);
std::string jump_code(const std::string& jump);

std::string encode_a_instruction(int value);
std::string encode_c_instruction(const CInstruction& instruction);
