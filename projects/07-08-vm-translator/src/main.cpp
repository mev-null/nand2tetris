#include <iostream>

// vm_translator Prog.vm writes Prog.asm next to the source.
// vm_translator Dir writes Dir/Dir.asm from every .vm file in Dir.
int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "usage: " << argv[0] << " Prog.vm | Dir" << std::endl;
    return 2;
  }

  std::cerr << "vm_translator: not implemented yet" << std::endl;
  return 1;
}
