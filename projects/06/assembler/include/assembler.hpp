#pragma once

#include <filesystem>
#include <istream>
#include <string>
#include <symbol_table.hpp>
#include <vector>

class Assembler {
 public:
  void AssembleFile(const std::filesystem::path& input_path,
                    const std::filesystem::path& output_path);

 private:
  void FirstPass(std::istream& input);
  void SecondPass(std::ostream& output);

  std::string ProcessAInstruction(const std::string& instruction);
  int ResolveAddress(const std::string& symbol);

  SymbolTable symbols_;
  int variable_symbol_address_ = 16;
  std::vector<std::string> instructions_;
};

// In GoogleTest scope
std::string encode_a_instruction(const std::string& instruction);
std::string encode_c_instruction(const std::string& instruction);
