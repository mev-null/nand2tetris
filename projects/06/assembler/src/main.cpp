#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "assembler.hpp"
#include "code.hpp"
#include "parser.hpp"
#include "symbol_table.hpp"

bool IsNumber(const std::string& symbol);

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " Prog.asm" << std::endl;
    return 2;
  }

  std::filesystem::path input_path = argv[1];
  std::ifstream file(input_path);
  if (!file) {
    std::cerr << "Failed to open " << input_path.string() << std::endl;
    return 1;
  }

  // Prog.asm -> Prog.hack in the same directory
  std::filesystem::path output_path = input_path;
  output_path.replace_extension(".hack");
  std::ofstream output(output_path);
  if (!output) {
    std::cerr << "Failed to create " << output_path.string() << std::endl;
    return 1;
  }

  SymbolTable symbols;
  // path 1
  std::vector<std::string> instructions = LoadInstructions(file, symbols);

  // path 2
  size_t cur_address = 0;
  size_t variable_symbol_address = 16;

  while (cur_address < instructions.size()) {
    std::string instruction = instructions[cur_address];
    InstructionType type = instruction_type(instruction);
    std::string machine_lang;
    switch (type) {
      case InstructionType::A_INSTRUCTION: {
        std::string a_symbol = parse_symbol(instruction);
        if (symbols.Contains(a_symbol)) {
          int a_value = symbols.GetAddress(a_symbol);
          instruction = '@' + std::to_string(a_value);
        } else if (!IsNumber(a_symbol)) {
          symbols.AddEntry(a_symbol, variable_symbol_address);
          instruction = '@' + std::to_string(variable_symbol_address);
          ++variable_symbol_address;
        }
        machine_lang = encode_a_instruction(instruction);
        ++cur_address;
        break;
      }
      case InstructionType::C_INSTRUCTION: {
        machine_lang = encode_c_instruction(instruction);
        ++cur_address;
        break;
      }
      case InstructionType::L_INSTRUCTION: {
        std::string jump_symbol = parse_symbol(instruction);
        if (symbols.Contains(jump_symbol)) {
          cur_address = symbols.GetAddress(jump_symbol);
          break;
        } else {
          throw std::invalid_argument("symbol" + jump_symbol + "not found: " + instruction);
        }
      }
      default: {
        throw std::invalid_argument("A instruction value out of range");
      }
    }
    if (!machine_lang.empty()) {
      output << machine_lang << '\n';
    } else {
      continue;
    }
  }
}

bool IsNumber(const std::string& symbol) {
  if (symbol.empty()) {
    return false;
  }

  return std::all_of(symbol.begin(), symbol.end(), [](unsigned char c) { return std::isdigit(c); });
}
