#include "code_writer.hpp"

#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "command.hpp"

namespace hack::vm {

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

}  // namespace hack::vm
