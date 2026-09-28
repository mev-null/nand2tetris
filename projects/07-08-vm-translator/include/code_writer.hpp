#pragma once

#include <ostream>
#include <string>
#include <string_view>

#include "absl/status/status.h"
#include "command.hpp"

namespace hack::vm {

class CodeWriter {
 public:
  CodeWriter(std::ostream& output, const std::string& filename)
      : output_(output), file_name_(filename) {}

  absl::Status WriteCommand(const Command& command);
  void WriteInfiniteLoop();

 private:
  // gate for each commands
  absl::Status WriteArithmetic(const Command& command);
  absl::Status WritePush(const Command& command);
  absl::Status WritePop(const Command& command);

  // arithmetic helper
  void WriteBinary(std::string_view comp);
  void WriteUnary(std::string_view comp);
  void WriteComparison(std::string_view jump_mnemonic);

  // pop/push helper
  void WritePushD();
  void WritePopToR13Address();
  void WritePushBaseAddress(std::string_view comp, int index);
  void WritePopBaseAddress(std::string_view comp, int index);

  // util
  std::string NewLabel(std::string_view kind);

  std::ostream& output_;
  std::string file_name_;
  std::string current_function_;
  int label_counter_ = 0;
};

}  // namespace hack::vm
