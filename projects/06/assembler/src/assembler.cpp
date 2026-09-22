#include "assembler.hpp"

#include <bitset>
#include <filesystem>
#include <istream>
#include <stdexcept>
#include <string>
#include <vector>

#include "code.hpp"
#include "parser.hpp"
#include "symbol_table.hpp"

bool _isNumber(const std::string& symbol);

std::string encode_a_instruction(const std::string& instruction) {
  std::string symbol = parse_symbol(instruction);

  int value = std::stoi(symbol);

  if (value < 0 || value > 32767) {
    throw std::invalid_argument("A instruction value out of range");
  }

  std::string result = "0";
  result += std::bitset<15>(value).to_string();
  return result;
}

std::string encode_c_instruction(const std::string& instruction) {
  CInstruction parsed = parse_c_instruction(instruction);
  std::string result = "111";
  result += comp_code(parsed.comp);
  result += dest_code(parsed.dest);
  result += jump_code(parsed.jump);

  return result;
}

std::vector<std::string> LoadInstructions(std::istream& input, SymbolTable& symbols) {
  std::string line;
  std::vector<std::string> instructions;
  int rom_address = 0;

  while (std::getline(input, line)) {
    std::string processed_line = process_line(line);
    if (!processed_line.empty()) {
      InstructionType type = instruction_type(processed_line);
      if (type == InstructionType::L_INSTRUCTION) {
        std::string parsed_symbol = parse_symbol(processed_line);
        symbols.AddEntry(parsed_symbol, rom_address);
      } else if (type == InstructionType::A_INSTRUCTION) {
        instructions.emplace_back(processed_line);
        ++rom_address;
      } else if (type == InstructionType::C_INSTRUCTION) {
        instructions.emplace_back(processed_line);
        ++rom_address;
      } else {
        throw std::invalid_argument("Invalid Instruciton: " + processed_line);
      }
    }
  }
  return instructions;
}

std::string EncodeInstruction(const std::string& instruction, SymbolTable& symbols,
                              int& next_variable_address);

void AssembleFile(const std::filesystem::path& input_path,
                  const std::filesystem::path& output_path);

bool _isNumber(const std::string& symbol) {
  if (symbol.empty()) {
    return false;
  }

  return std::all_of(symbol.begin(), symbol.end(), [](unsigned char c) { return std::isdigit(c); });
}
