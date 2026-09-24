#include "parser.hpp"

#include <charconv>
#include <cstddef>
#include <sstream>
#include <string>
#include <system_error>
#include <unordered_map>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
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

const std::unordered_map<std::string, Segment> kSegmentTable{
    {"argument", Segment::kArgument},
    {"local",    Segment::kLocal   },
    {"static",   Segment::kStatic  },
    {"constant", Segment::kConstant},
    {"this",     Segment::kThis    },
    {"that",     Segment::kThat    },
    {"pointer",  Segment::kPointer },
    {"temp",     Segment::kTemp    },
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

absl::StatusOr<Command> ParseCommand(const std::string& line) {
  std::istringstream stream(line);

  std::string name;
  if (!(stream >> name)) {
    return absl::InvalidArgumentError("no command on the line");
  }

  const auto entry = kCommandTable.find(name);
  if (entry == kCommandTable.end()) {
    return absl::InvalidArgumentError(absl::StrCat("unknown command: ", line));
  }

  Command command;
  command.type = entry->second;

  std::string arg1;
  if (command.type == CommandType::kArithmetic) {
    if (stream >> arg1) {
      return absl::InvalidArgumentError(absl::StrCat(name, " takes no argument: ", line));
    }
    command.arg1 = name;
  } else {
    if (stream >> arg1) {
      const auto seg_entry = kSegmentTable.find(arg1);
      if (seg_entry == kSegmentTable.end()) {
        return absl::InvalidArgumentError(absl::StrCat("unknown segment: ", arg1));
      }
      command.segment = seg_entry->second;

      std::string arg2;
      if (stream >> arg2) {
        int index = 0;
        const char* last = arg2.data() + arg2.size();
        const auto [stop, error] = std::from_chars(arg2.data(), last, index);
        if (error != std::errc{} || stop != last) {
          return absl::InvalidArgumentError(
              absl::StrCat(name, " takes a number as its index: ", line));
        }
        if (index < 0) {
          return absl::InvalidArgumentError(
              absl::StrCat(name, " takes a non-negative index: ", line));
        }
        command.arg2 = index;
      } else {
        return absl::InvalidArgumentError(
            absl::StrCat(name, " takes a segment and an index: ", line));
      }
      std::string arg3;
      if (stream >> arg3) {
        return absl::InvalidArgumentError(
            absl::StrCat(name, " takes a segment and an index: ", line));
      }
    } else {
      return absl::InvalidArgumentError(
          absl::StrCat(name, " takes a segment and an index: ", line));
    }
  }
  return command;
}

}  // namespace hack::vm
