#include "code_writer.hpp"

#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "command.hpp"

namespace hack::vm {

absl::Status CodeWriter::WriteCommand(const Command& command) {
  switch (command.type) {
    case CommandType::kArithmetic:
      return WriteArithmetic(command);
    case CommandType::kPush:
    case CommandType::kPop:
      return WritePushPop(command);
    default:
      return absl::UnimplementedError("command type is not translated yet");
  }
}

void CodeWriter::WriteInfiniteLoop() {
  output_ << "(__VM_END)\n"
          << "@__VM_END\n"
          << "0;JMP\n";
}

absl::Status CodeWriter::WriteArithmetic(const Command& command) {
  return absl::UnimplementedError("This function is not implemented yet");
}

absl::Status CodeWriter::WritePushPop(const Command& command) {
  if (!command.segment.has_value()) {
    return absl::InvalidArgumentError("push/pop requires a segment");
  }
  if (!command.arg2.has_value()) {
    return absl::InvalidArgumentError("push/pop requires a index");
  }
  switch (*command.segment) {
    case Segment::kConstant: {
      output_ << "@" << *command.arg2 << "\n"
              << "D=A\n";
      break;
    }
    case Segment::kLocal: {
      output_ << "@LCL\n"
              << "D=M\n"
              << "@" << *command.arg2 << "\n"
              << "D=D+A\n"
              << "A=D\n"
              << "D=M\n";
      break;
    }
    default:
      return absl::UnimplementedError("This segment is not implemented yet");
  }
  PushDToStack();
  return absl::OkStatus();
}

void CodeWriter::PushDToStack() {
  output_ << "@SP\n"
          << "A=M\n"
          << "M=D\n"
          << "@SP\n"
          << "M=M+1\n";
}

void CodeWriter::PopStackToD() {}

std::string CodeWriter::NewLabel(std::string_view prefix) {
  throw std::invalid_argument("This function is not implemented yet");
}

}  // namespace hack::vm
