#include "code_writer.hpp"

#include <ostream>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "command.hpp"

namespace hack::vm {

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
