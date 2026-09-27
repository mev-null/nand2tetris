#include "vm_translator.hpp"

#include <filesystem>
#include <fstream>
#include <string>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_cat.h"
#include "code_writer.hpp"
#include "command.hpp"
#include "parser.hpp"

namespace hack::vm {

absl::Status TranslateFile(const std::filesystem::path& input_path,
                           const std::filesystem::path& output_path) {
  std::ifstream input(input_path);
  if (!input) {
    return absl::NotFoundError(absl::StrCat("file not found: ", input_path.string()));
  }

  std::ofstream output(output_path);
  if (!output) {
    return absl::InternalError(absl::StrCat("failed to create file: ", output_path.string()));
  }

  CodeWriter writer(output, input_path.stem().string());

  std::string line;
  while (std::getline(input, line)) {
    std::string processed_line = ProcessLine(line);
    if (processed_line.empty()) {
      continue;
    }
    absl::StatusOr<Command> command = ParseCommand(processed_line);
    if (!command.ok()) {
      return command.status();
    }
    absl::Status write_command = writer.WriteCommand(*command);
    if (!write_command.ok()) {
      return write_command;
    }
  }
  writer.WriteInfiniteLoop();
  return absl::OkStatus();
}

}  // namespace hack::vm
