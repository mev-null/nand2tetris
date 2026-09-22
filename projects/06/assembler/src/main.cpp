#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#include "assembler.hpp"
#include "code.hpp"
#include "parser.hpp"

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

  std::string line;

  while (std::getline(file, line)) {
    std::string processed_line = process_line(line);
    if (!processed_line.empty()) {
      InstructionType type = instruction_type(processed_line);
      std::string machine_lang;
      switch (type) {
        case InstructionType::A_INSTRUCTION: {
          machine_lang = encode_a_instruction(processed_line);
          break;
        }
        case InstructionType::C_INSTRUCTION: {
          machine_lang = encode_c_instruction(processed_line);
          break;
        }
        case InstructionType::L_INSTRUCTION: {
          // a label does not occupy a ROM word
          continue;
        }
        default: {
          throw std::invalid_argument("A instruction value out of range");
        }
      }
      output << machine_lang << '\n';
    }
  }
}
