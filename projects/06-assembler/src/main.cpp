#include <filesystem>
#include <iostream>

#include "assembler.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " Prog.asm" << std::endl;
    return 2;
  }

  std::filesystem::path input_path = argv[1];
  std::filesystem::path output_path = input_path;
  output_path.replace_extension(".hack");
  Assembler r;
  r.AssembleFile(input_path, output_path);
  return 0;
}
