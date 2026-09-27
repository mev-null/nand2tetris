#include <cstdlib>
#include <filesystem>
#include <iostream>

#include "absl/status/status.h"
#include "vm_translator.hpp"

// vm_translator Prog.vm writes Prog.asm next to the source.
// vm_translator Dir writes Dir/Dir.asm from every .vm file in Dir.
int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " Prog.vm | Dir" << std::endl;
    return 2;
  }
  std::filesystem::path input_path = argv[1];

  std::filesystem::path output_path = input_path;
  output_path.replace_extension(".asm");

  absl::Status status = hack::vm::TranslateFile(input_path, output_path);

  if (!status.ok()) {
    std::cerr << status << "\n";
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
