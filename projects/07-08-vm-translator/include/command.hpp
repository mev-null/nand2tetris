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

enum class Operator {
  kAdd,
  kSub,
  kNeg,
  kEq,
  kGt,
  kLt,
  kAnd,
  kOr,
  kNot,
};

struct Command {
  CommandType type;
  std::optional<Operator> op;       // arithmetic
  std::optional<std::string> arg1;  // label / function name
  std::optional<Segment> segment;   // push / pop
  std::optional<int> arg2;
};

}  // namespace hack::vm
