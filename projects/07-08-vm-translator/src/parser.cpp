#include "parser.hpp"

#include <cstddef>
#include <exception>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "command.hpp"

namespace hack::vm {

namespace {

// add more types in chapter 8
const std::unordered_map<std::string, CommandType> kCommandTable{
    {"push", CommandType::kPush      },
    {"pop",  CommandType::kPop       },

    {"add",  CommandType::kArithmetic},
    {"sub",  CommandType::kArithmetic},
    {"neg",  CommandType::kArithmetic},
    {"eq",   CommandType::kArithmetic},
    {"gt",   CommandType::kArithmetic},
    {"lt",   CommandType::kArithmetic},
    {"and",  CommandType::kArithmetic},
    {"or",   CommandType::kArithmetic},
    {"not",  CommandType::kArithmetic},
};

}  // namespace

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
  std::istringstream stream(line);

  std::string name;
  if (!(stream >> name)) {
    throw std::invalid_argument("no command on the line");
  }

  const auto entry = kCommandTable.find(name);
  if (entry == kCommandTable.end()) {
    throw std::invalid_argument("unknown command: " + line);
  }

  Command command;
  command.type = entry->second;

  std::string arg1;
  if (command.type == CommandType::kArithmetic) {
    if (stream >> arg1) {
      throw std::invalid_argument(name + " takes no argument: " + line);
    }
    command.arg1 = name;
  } else {
    if (stream >> arg1) {
      command.arg1 = arg1;

      std::string arg2;
      if (stream >> arg2) {
        try {
          command.arg2 = std::stoi(arg2);
        } catch (const std::exception&) {
          throw std::invalid_argument(name + " takes a number as its index: " + line);
        }
      } else {
        throw std::invalid_argument(name + " takes a segment and an index: " + line);
      }
      std::string arg3;
      if (stream >> arg3) {
        throw std::invalid_argument(name + " takes a segment and an index: " + line);
      }
    } else {
      throw std::invalid_argument(name + " takes a segment and an index: " + line);
    }
  }
  return command;
}

}  // namespace hack::vm
