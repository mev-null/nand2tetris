#include "vm_translator.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/status/status.h"
#include "absl/status/status_matchers.h"

namespace hack::vm {
namespace {

using ::absl_testing::IsOk;
using ::absl_testing::StatusIs;
using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::Not;
using ::testing::StartsWith;

std::filesystem::path WriteVmFile(const std::string& name, const std::string& contents) {
  std::filesystem::path path = std::filesystem::path(::testing::TempDir()) / name;
  std::ofstream(path) << contents;
  return path;
}

std::filesystem::path AsmPath(const std::filesystem::path& vm_path) {
  std::filesystem::path path = vm_path;
  path.replace_extension(".asm");
  return path;
}

std::string ReadFile(const std::filesystem::path& path) {
  std::ifstream input(path);
  std::ostringstream contents;
  contents << input.rdbuf();
  return contents.str();
}

TEST(TranslateFileTest, TranslatesValidProgram) {
  std::filesystem::path input = WriteVmFile("Valid.vm", "push constant 7\npush constant 8\nadd\n");

  EXPECT_THAT(TranslateFile(input, AsmPath(input)), IsOk());
}

TEST(TranslateFileTest, ReturnsNotFoundForMissingInput) {
  std::filesystem::path input = std::filesystem::path(::testing::TempDir()) / "Missing.vm";

  EXPECT_THAT(TranslateFile(input, AsmPath(input)), StatusIs(absl::StatusCode::kNotFound));
}

// Each command is preceded by its source line, without the source comment.
TEST(TranslateFileTest, WritesEachCommandAsComment) {
  std::filesystem::path input =
      WriteVmFile("Commented.vm", "// header\npush constant 7 // seven\n\nadd\n");

  ASSERT_THAT(TranslateFile(input, AsmPath(input)), IsOk());
  std::string output = ReadFile(AsmPath(input));
  EXPECT_THAT(output, HasSubstr("// push constant 7\n@7\n"));
  EXPECT_THAT(output, HasSubstr("// add\n@SP\n"));
  EXPECT_THAT(output, Not(HasSubstr("header")));
  EXPECT_THAT(output, Not(HasSubstr("seven")));
}

// Blank and comment lines count toward the line number.
TEST(TranslateFileTest, PrefixesParseErrorWithFileAndLine) {
  std::filesystem::path input =
      WriteVmFile("ParseError.vm", "// comment\n\npush constant 7\naddi\n");

  EXPECT_THAT(TranslateFile(input, AsmPath(input)),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       AllOf(StartsWith("ParseError.vm:4: "), HasSubstr("unknown command"))));
}

TEST(TranslateFileTest, PrefixesWriteErrorWithFileAndLine) {
  std::filesystem::path input = WriteVmFile("WriteError.vm", "push constant 7\npop constant 0\n");

  EXPECT_THAT(TranslateFile(input, AsmPath(input)),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       AllOf(StartsWith("WriteError.vm:2: "), HasSubstr("constant"))));
}

}  // namespace
}  // namespace hack::vm
