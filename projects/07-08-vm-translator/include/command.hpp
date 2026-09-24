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

enum class Segment {
  kArgument,
  kLocal,
  kStatic,
  kConstant,
  kThis,
  kThat,
  kPointer,
  kTemp,
};

struct Command {
  CommandType type;
  std::optional<std::string> arg1;  // arithmetic op / label / function name
  std::optional<Segment> segment;   // push / pop
  std::optional<int> arg2;
};

}  // namespace hack::vm
