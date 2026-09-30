#include "code_writer.hpp"

#include <optional>
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
    case Segment::kLocal:
      WritePushBaseAddress("LCL", *command.arg2);
      break;
    case Segment::kArgument:
      WritePushBaseAddress("ARG", *command.arg2);
      break;
    case Segment::kThis:
      WritePushBaseAddress("THIS", *command.arg2);
      break;
    case Segment::kThat:
      WritePushBaseAddress("THAT", *command.arg2);
      break;
    case Segment::kTemp: {
      int index = *command.arg2;
      if ((index < 0) || (index > 7)) {
        return absl::InvalidArgumentError("");
      }
      WritePushWithSymbol("TEMP", index);
      return absl::OkStatus();
    }
    case Segment::kPointer: {
      int index = *command.arg2;
      if ((index != 0) && (index != 1)) {
        return absl::InvalidArgumentError("");
      }
      WritePushWithSymbol("PTR", index);
      return absl::OkStatus();
    }
    case Segment::kStatic:
      WritePushStatic(*command.arg2);
      return absl::OkStatus();
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
    case Segment::kLocal:
      WritePopBaseAddress("LCL", *command.arg2);
      break;
    case Segment::kArgument:
      WritePopBaseAddress("ARG", *command.arg2);
      break;
    case Segment::kThis:
      WritePopBaseAddress("THIS", *command.arg2);
      break;
    case Segment::kThat:
      WritePopBaseAddress("THAT", *command.arg2);
      break;
    case Segment::kConstant:
      return absl::InvalidArgumentError("cannot pop to constant segment");
    case Segment::kTemp: {
      int index = *command.arg2;
      if ((index < 0) || (index > 7)) {
        return absl::InvalidArgumentError("temp index expected 0 to 7");
      }
      WritePopWithSymbol("TEMP", index);
      return absl::OkStatus();
    }
    case Segment::kPointer: {
      int index = *command.arg2;
      if ((index != 0) && (index != 1)) {
        return absl::InvalidArgumentError("pointer indext expected 0 or 7");
      }
      WritePopWithSymbol("PTR", index);
      return absl::OkStatus();
    }
    case Segment::kStatic:
      WritePopStatic(*command.arg2);
      return absl::OkStatus();
    default:
      return absl::UnimplementedError("this segment is not implemented yet");
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
  std::string new_label = NewLabel("CMP", std::nullopt);
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

void CodeWriter::WritePopWithSymbol(std::string_view comp, int index) {
  std::string symbol;
  if (comp == "TEMP") {
    symbol = CreateSymbol(Segment::kTemp, index);
  } else if (comp == "PTR") {
    symbol = CreateSymbol(Segment::kPointer, index);
  }
  output_ << "@SP\n"
          << "AM=M-1\n"
          << "D=M\n"
          << symbol << "\n"
          << "M=D\n";
}

void CodeWriter::WritePushWithSymbol(std::string_view comp, int index) {
  std::string symbol;
  if (comp == "TEMP") {
    symbol = CreateSymbol(Segment::kTemp, index);
  } else if (comp == "PTR") {
    symbol = CreateSymbol(Segment::kPointer, index);
  }
  output_ << symbol << "\n"
          << "D=M\n"
          << "@SP\n"
          << "A=M\n"
          << "M=D\n"
          << "@SP\n"
          << "M=M+1\n";
}

void CodeWriter::WritePopStatic(int index) {
  std::string new_label = NewLabel("STATIC", index);

  output_ << "@SP\n"
          << "AM=M-1\n"
          << "D=M\n"
          << "@" << new_label << "\n"
          << "M=D\n";
}

void CodeWriter::WritePushStatic(int index) {
  std::string new_label = NewLabel("STATIC", index);

  output_ << "@" << new_label << "\n"
          << "D=M\n"
          << "@SP\n"
          << "A=M\n"
          << "M=D\n"
          << "@SP\n"
          << "M=M+1\n";
}

std::string CodeWriter::NewLabel(std::string_view kind, std::optional<int> index) {
  std::string label;
  if ((kind == "CMP") && !index) {
    label = absl::StrCat(file_name_, ".", kind, ".", label_counter_);
    ++label_counter_;
  } else if ((kind == "STATIC") && index) {
    label = absl::StrCat(file_name_, ".", *index);
  }
  return label;
}

std::string CodeWriter::CreateSymbol(Segment segment, int index) {
  switch (segment) {
    case Segment::kTemp: {
      return absl::StrCat("@R", 5 + index);
    }
    case Segment::kPointer: {
      switch (index) {
        case 0:
          return "@THIS";
        case 1:
          return "@THAT";
        default:
          return "";
      }
    }
    default:
      return "";
  }
}

}  // namespace hack::vm
