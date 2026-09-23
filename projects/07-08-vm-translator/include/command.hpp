#pragma once

#include <optional>
#include <string>

namespace hack::vm {

enum class CommandType {
  kArithmetic,
  kPush,
  kPop,
  kLabel,
  kGoto,
  kIf,
  kFunction,
  kReturn,
  kCall,
};

struct Command {
  CommandType type;
  std::optional<std::string> arg1;
  std::optional<int> arg2;
};

}  // namespace hack::vm
