#include "parser.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

#include "command.hpp"

namespace hack::vm {

std::string ProcessLine(std::string line) {
  std::size_t com_pos = line.find("//");
  line = line.substr(0, com_pos);

  std::size_t pos = 0;
  while ((pos < line.size()) && (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\r')) {
    ++pos;
  }
  line = line.substr(pos);

  std::size_t rpos = line.size();
  while ((rpos > 0) &&
         (line[rpos - 1] == ' ' || line[rpos - 1] == '\t' || line[rpos - 1] == '\r')) {
    --rpos;
  }
  line = line.substr(0, rpos);

  return line;
}

Command ParseCommand(const std::string& line) {
  throw std::logic_error("ParseCommand is not implemented");
}

}  // namespace hack::vm
