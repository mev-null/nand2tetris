#include "code_writer.hpp"

#include <ostream>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "command.hpp"

namespace hack::vm {

absl::Status CodeWriter::WriteCommand(const Command& command) {
  switch (command.type) {
    case CommandType::kArithmetic:
      return WriteArithmetic(command);
    case CommandType::kPush:
    case CommandType::kPop:
      if (!command.segment.has_value()) {
        return absl::InvalidArgumentError("push/pop requires a segment");
      }
      if (!command.arg2.has_value()) {
        return absl::InvalidArgumentError("push/pop requires an index");
      }
      if (command.type == CommandType::kPush) {
        return WritePush(*command.segment, *command.arg2);
      }
      return WritePop(*command.segment, *command.arg2);
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

absl::Status CodeWriter::WritePush(Segment segment, int index) {
  switch (segment) {
    case Segment::kConstant: {
      output_ << "@" << index << "\n"
              << "D=A\n";
      break;
    }
    case Segment::kLocal:
      WritePushBaseAddress("LCL", index);
      break;
    case Segment::kArgument:
      WritePushBaseAddress("ARG", index);
      break;
    case Segment::kThis:
      WritePushBaseAddress("THIS", index);
      break;
    case Segment::kThat:
      WritePushBaseAddress("THAT", index);
      break;
    case Segment::kTemp:
    case Segment::kPointer:
    case Segment::kStatic: {
      absl::StatusOr<std::string> symbol = CreateSymbol(segment, index);
      if (!symbol.ok()) {
        return symbol.status();
      }
      WritePushSymbol(*symbol);
      break;
    }
  }
  WritePushD();
  return absl::OkStatus();
}

absl::Status CodeWriter::WritePop(Segment segment, int index) {
  switch (segment) {
    case Segment::kLocal:
      WritePopBaseAddress("LCL", index);
      break;
    case Segment::kArgument:
      WritePopBaseAddress("ARG", index);
      break;
    case Segment::kThis:
      WritePopBaseAddress("THIS", index);
      break;
    case Segment::kThat:
      WritePopBaseAddress("THAT", index);
      break;
    case Segment::kTemp:
    case Segment::kPointer:
    case Segment::kStatic: {
      absl::StatusOr<std::string> symbol = CreateSymbol(segment, index);
      if (!symbol.ok()) {
        return symbol.status();
      }
      WritePopSymbol(*symbol);
      return absl::OkStatus();
    }
    case Segment::kConstant:
      return absl::InvalidArgumentError("cannot pop to constant segment");
  }
  WritePopToR13Address();
  return absl::OkStatus();
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

void CodeWriter::WritePushBaseAddress(std::string_view comp, int index) {
  output_ << "@" << comp << "\n"
          << "D=M\n"
          << "@" << index << "\n"
          << "D=D+A\n"
          << "A=D\n"
          << "D=M\n";
}

void CodeWriter::WritePopBaseAddress(std::string_view comp, int index) {
  output_ << "@" << comp << "\n"
          << "D=M\n"
          << "@" << index << "\n"
          << "D=D+A\n"
          << "@R13\n"
          << "M=D\n";
}

void CodeWriter::WritePushSymbol(std::string_view symbol) {
  output_ << "@" << symbol << "\n"
          << "D=M\n";
}

void CodeWriter::WritePopSymbol(std::string_view symbol) {
  output_ << "@SP\n"
          << "AM=M-1\n"
          << "D=M\n"
          << "@" << symbol << "\n"
          << "M=D\n";
}

std::string CodeWriter::NewLabel(std::string_view kind) {
  std::string label = absl::StrCat(file_name_, ".", kind, ".", label_counter_);
  ++label_counter_;
  return label;
}

absl::StatusOr<std::string> CodeWriter::CreateSymbol(Segment segment, int index) {
  switch (segment) {
    case Segment::kTemp: {
      if (index < 0 || index > 7) {
        return absl::InvalidArgumentError(absl::StrCat("temp index out of range: ", index));
      }
      return absl::StrCat("R", 5 + index);
    }
    case Segment::kPointer: {
      switch (index) {
        case 0:
          return "THIS";
        case 1:
          return "THAT";
        default:
          return absl::InvalidArgumentError(absl::StrCat("pointer index out of range: ", index));
      }
    }
    case Segment::kStatic: {
      return absl::StrCat(file_name_, ".", index);
    }
    default:
      return absl::InternalError("CreateSymbol requires temp, pointer, or static");
  }
}

}  // namespace hack::vm
