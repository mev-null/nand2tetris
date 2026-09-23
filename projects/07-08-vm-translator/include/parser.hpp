#pragma once

#include <string>

#include "command.hpp"

namespace hack::vm {

std::string ProcessLine(std::string line);
Command ParseCommand(const std::string& line);

}  // namespace hack::vm
