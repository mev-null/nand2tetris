#pragma once

#include <filesystem>

#include "absl/status/status.h"

namespace hack::vm {

absl::Status TranslateFile(const std::filesystem::path& input_path,
                           const std::filesystem::path& output_path);

}  // namespace hack::vm
