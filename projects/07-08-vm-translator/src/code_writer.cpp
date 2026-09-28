#include "code_writer.hpp"

#include <ostream>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "command.hpp"

namespace hack::vm {

absl::Status CodeWriter::WriteCommand(const Command& command) {
  switch (command.type) {
    case CommandType::kArithmetic:
      return WriteArithmetic(command);
    case CommandType::kPush:
      return WritePush(command);
    case CommandType::kPop:
      return WritePop(command);
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
  if (!command.op.has_value()) {
    return absl::InvalidArgumentError("arithmetic requires an operator");
  }
  switch (*command.op) {
    case Operator::kAdd:
      WriteBinary("D+M");
      break;
    case Operator::kSub:
      WriteBinary("M-D");
      break;
    case Operator::kAnd:
      WriteBinary("D&M");
      break;
    case Operator::kOr:
      WriteBinary("D|M");
      break;
    case Operator::kNeg:
      WriteUnary("-M");
      break;
    case Operator::kNot:
      WriteUnary("!M");
      break;
    case Operator::kEq:
      WriteComparison("JEQ");
      break;
    case Operator::kGt:
      WriteComparison("JGT");
      break;
    case Operator::kLt:
      WriteComparison("JLT");
      break;
  }
  return absl::OkStatus();
}

absl::Status CodeWriter::WritePush(const Command& command) {
  if (!command.segment.has_value()) {
    return absl::InvalidArgumentError("push/pop requires a segment");
  }
  if (!command.arg2.has_value()) {
    return absl::InvalidArgumentError("push/pop requires an index");
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
      return absl::UnimplementedError("this segment is not implemented yet");
  }
  WritePushD();
  return absl::OkStatus();
}

absl::Status CodeWriter::WritePop(const Command& command) {
  if (!command.segment.has_value()) {
    return absl::InvalidArgumentError("push/pop requires a segment");
  }
  if (!command.arg2.has_value()) {
    return absl::InvalidArgumentError("push/pop requires an index");
  }
  switch (*command.segment) {
    case Segment::kLocal: {
      output_ << "@LCL\n"
              << "D=M\n"
              << "@" << *command.arg2 << "\n"
              << "D=D+A\n"
              << "@R13\n"
              << "M=D\n";
      break;
    }
    case Segment::kConstant:
      return absl::InvalidArgumentError("cannot pop to constant segment");
    default:
      return absl::UnimplementedError("this segment is not implemented yet");
  }
  WritePopToR13Address();
  return absl::OkStatus();
}

void CodeWriter::WritePushD() {
  output_ << "@SP\n"
          << "A=M\n"
          << "M=D\n"
          << "@SP\n"
          << "M=M+1\n";
}

void CodeWriter::WritePopToR13Address() {
  output_ << "@SP\n"
          << "M=M-1\n"
          << "A=M\n"
          << "D=M\n"
          << "@R13\n"
          << "A=M\n"
          << "M=D\n";
}

void CodeWriter::WriteBinary(std::string_view comp) {
  output_ << "@SP\n"
          << "AM=M-1\n"
          << "D=M\n"
          << "A=A-1\n"
          << "M=" << comp << "\n";
}

void CodeWriter::WriteUnary(std::string_view comp) {
  output_ << "@SP\n"
          << "A=M-1\n"
          << "M=" << comp << "\n";
}

void CodeWriter::WriteComparison(std::string_view jump_mnemonic) {
  std::string new_label = NewLabel("CMP");
  std::string true_label = absl::StrCat(new_label, ".TRUE");
  std::string end_label = absl::StrCat(new_label, ".END");

  output_ << "@SP\n"
          << "AM=M-1\n"
          << "D=M\n"
          << "A=A-1\n"
          << "D=M-D\n"
          << "@" << true_label << "\n"
          << "D;" << jump_mnemonic
          << "\n"
          // false path
          << "@SP\n"
          << "A=M-1\n"
          << "M=0\n"
          << "@" << end_label << "\n"
          << "0;JMP\n"
          // true path
          << "(" << true_label << ")\n"
          << "@SP\n"
          << "A=M-1\n"
          << "M=-1\n"
          // end
          << "(" << end_label << ")\n";
}

std::string CodeWriter::NewLabel(std::string_view kind) {
  std::string label = absl::StrCat(file_name_, ".", kind, ".", label_counter_);
  ++label_counter_;
  return label;
}

}  // namespace hack::vm
