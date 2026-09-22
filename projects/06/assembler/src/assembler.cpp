#include "assembler.hpp"

#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
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

void Assembler::AssembleFile(const std::filesystem::path& input_path,
                             const std::filesystem::path& output_path) {
  std::ifstream file(input_path);
  if (!file) {
    std::cerr << "Failed to open " << input_path.string() << std::endl;
  }

  std::ofstream output(output_path);
  if (!output) {
    std::cerr << "Failed to create " << output_path.string() << std::endl;
  }

  FirstPass(file);
  SecondPass(output);
}

void Assembler::FirstPass(std::istream& input) {
  std::string line;
  int rom_address = 0;

  while (std::getline(input, line)) {
    std::string processed_line = process_line(line);
    if (!processed_line.empty()) {
      InstructionType type = instruction_type(processed_line);
      if (type == InstructionType::A_INSTRUCTION || type == InstructionType::C_INSTRUCTION) {
        instructions_.emplace_back(processed_line);
        ++rom_address;
      } else if (type == InstructionType::L_INSTRUCTION) {
        std::string parsed_symbol = parse_symbol(processed_line);
        symbols_.AddEntry(parsed_symbol, rom_address);
      } else {
        throw std::invalid_argument("Invalid Instruciton: " + processed_line);
      }
    }
  }
}

void Assembler::SecondPass(std::ostream& output) {
  std::string machine_lang;
  int cur_address = 0;
  while (cur_address < instructions_.size()) {
    std::string instruction = instructions_[cur_address];
    InstructionType type = instruction_type(instruction);
    switch (type) {
      case InstructionType::A_INSTRUCTION: {
        machine_lang = ProcessAInstruction(instruction);
        ++cur_address;
        break;
      }
      case InstructionType::C_INSTRUCTION: {
        machine_lang = encode_c_instruction(instruction);
        ++cur_address;
        break;
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

std::string Assembler::ProcessAInstruction(const std::string& instruction) {
  int address = ResolveAddress(parse_symbol(instruction));
  return encode_a_instruction('@' + std::to_string(address));
}

int Assembler::ResolveAddress(const std::string& symbol) {
  if (_isNumber(symbol)) {
    return std::stoi(symbol);
  }
  if (!symbols_.Contains(symbol)) {
    // neither predefined nor a label: a new variable
    symbols_.AddEntry(symbol, variable_symbol_address_);
    ++variable_symbol_address_;
  }
  return symbols_.GetAddress(symbol);
}

bool _isNumber(const std::string& symbol) {
  if (symbol.empty()) {
    return false;
  }

  return std::all_of(symbol.begin(), symbol.end(), [](unsigned char c) { return std::isdigit(c); });
}
