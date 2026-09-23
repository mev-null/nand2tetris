#include "parser.hpp"

#include <stdexcept>
#include <string>

#include "command.hpp"

namespace hack::vm {

std::string ProcessLine(std::string line) {
  throw std::logic_error("ProcessLine is not implemented");
}

Command ParseCommand(const std::string& line) {
  throw std::logic_error("ParseCommand is not implemented");
}

}  // namespace hack::vm
