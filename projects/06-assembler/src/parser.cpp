#include "parser.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

#include "instruction.hpp"

std::string process_line(std::string line) {
  std::size_t com_pos = line.find("//");
  line = line.substr(0, com_pos);

  std::size_t pos = 0;
  while ((pos < line.size()) && (line[pos] == ' ' || line[pos] == '\t' || line[pos] == '\r')) {
    ++pos;
  }
  line = line.substr(pos);

  std::size_t rpos = line.size();
  while ((rpos > 0) &&
         (line[rpos - 1] == ' ' || line[rpos - 1] == '\t' || line[rpos - 1] == '\r')) {
    --rpos;
  }
  line = line.substr(0, rpos);

  return line;
}

InstructionType instruction_type(const std::string& instruction) {
  if (instruction.empty()) {
    throw std::invalid_argument("instruction must not be empty");
  }

  if (instruction[0] == '@') {
    return InstructionType::A_INSTRUCTION;
  } else if (instruction[0] == '(') {
    return InstructionType::L_INSTRUCTION;
  } else {
    return InstructionType::C_INSTRUCTION;
  }
}

std::string parse_symbol(const std::string& instruction) {
  if (instruction.empty()) {
    throw std::invalid_argument("instruction must not be empty");
  }

  std::size_t size = instruction.size();
  if (instruction[0] == '@') {
    if (size < 2) {
      throw std::invalid_argument("Length of A instruction must be at least 2 letters");
    }
    return instruction.substr(1);
  } else if (instruction[0] == '(') {
    if (size < 3) {
      throw std::invalid_argument("Length of L instruction must be at least 3 letters");
    }
    return instruction.substr(1, size - 2);
  } else {
    throw std::invalid_argument("input must be A instruction or L instruction");
  }
}

CInstruction parse_c_instruction(const std::string& instruction) {
  std::size_t eqpos = instruction.find("=");
  std::size_t semipos = instruction.find(";");

  CInstruction result;

  if (eqpos != std::string::npos) {
    result.dest = instruction.substr(0, eqpos);
  }

  // parse comp
  std::size_t comp_start = 0;
  std::size_t comp_end = instruction.size() - 1;
  if (eqpos != std::string::npos) {
    comp_start = eqpos + 1;
  }
  if (semipos != std::string::npos) {
    comp_end = semipos - 1;
  }
  std::size_t comp_len = comp_end - comp_start + 1;
  result.comp = instruction.substr(comp_start, comp_len);

  if (semipos != std::string::npos) {
    result.jump = instruction.substr(semipos + 1);
  }

  return result;
}
