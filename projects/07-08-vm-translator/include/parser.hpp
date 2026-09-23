#pragma once

#include <string>

#include "absl/status/statusor.h"
#include "command.hpp"

namespace hack::vm {

std::string ProcessLine(std::string line);
absl::StatusOr<Command> ParseCommand(const std::string& line);

}  // namespace hack::vm
