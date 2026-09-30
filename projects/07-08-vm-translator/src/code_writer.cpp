#include "code_writer.hpp"

#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "command.hpp"

namespace hack::vm {

void CodeWriter::WriteSourceComment(std::string_view line) { output_ << "// " << line << "\n"; }

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

std::string CodeWriter::NewLabel(std::string_view kind) {
  std::string label = absl::StrCat(file_name_, ".", kind, ".", label_counter_);
  ++label_counter_;
  return label;
}

}  // namespace hack::vm
