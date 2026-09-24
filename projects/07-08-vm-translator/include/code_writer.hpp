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
  absl::Status WriteArithmetic(const Command& command);
  absl::Status WritePushPop(const Command& command);

  void PushDToStack();
  void PopStackToD();
  std::string NewLabel(std::string_view prefix);

  std::ostream& output_;
  std::string file_name_;
  std::string current_function_;
  int label_counter_ = 0;
};

}  // namespace hack::vm
