#include <fstream>
#include <iostream>
#include <string>

#include "parser.hpp"

int main() {
  std::cout << "filename: ";
  std::string filename;
  std::cin >> filename;

  std::ifstream file(filename);
  if (!file) {
    std::cerr << "Failed to open file" << std::endl;
    return 1;
  }

  std::string line;

  while (std::getline(file, line)) {
    std::string processed_line = process_line(line);
    if (!processed_line.empty()) {
      std::cout << processed_line << '\n';
    }
  }
}
