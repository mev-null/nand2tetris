#pragma once

#include <filesystem>
#include <string>
#include <symbol_table.hpp>

std::string encode_a_instruction(const std::string& instruction);
std::string encode_c_instruction(const std::string& instruction);

std::vector<std::string> LoadInstructions(std::istream& input, SymbolTable& symbols);

std::string EncodeInstruction(const std::string& instruction, SymbolTable& symbols,
                              int& next_variable_address);

void AssembleFile(const std::filesystem::path& input_path,
                  const std::filesystem::path& output_path);
