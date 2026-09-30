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

namespace {

absl::Status WithLocation(const absl::Status& status, const std::filesystem::path& path,
                          int line_number) {
  return absl::Status(status.code(), absl::StrCat(path.filename().string(), ":", line_number, ": ",
                                                  status.message()));
}

}  // namespace

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
  int line_number = 0;
  while (std::getline(input, line)) {
    ++line_number;
    std::string processed_line = ProcessLine(line);
    if (processed_line.empty()) {
      continue;
    }
    absl::StatusOr<Command> command = ParseCommand(processed_line);
    if (!command.ok()) {
      return WithLocation(command.status(), input_path, line_number);
    }
    absl::Status write_command = writer.WriteCommand(*command);
    if (!write_command.ok()) {
      return WithLocation(write_command, input_path, line_number);
    }
  }
  writer.WriteInfiniteLoop();
  return absl::OkStatus();
}

}  // namespace hack::vm
