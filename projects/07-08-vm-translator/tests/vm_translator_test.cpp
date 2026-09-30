#include "vm_translator.hpp"

#include <filesystem>
#include <fstream>
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

TEST(TranslateFileTest, TranslatesValidProgram) {
  std::filesystem::path input = WriteVmFile("Valid.vm", "push constant 7\npush constant 8\nadd\n");

  EXPECT_THAT(TranslateFile(input, AsmPath(input)), IsOk());
}

TEST(TranslateFileTest, ReturnsNotFoundForMissingInput) {
  std::filesystem::path input = std::filesystem::path(::testing::TempDir()) / "Missing.vm";

  EXPECT_THAT(TranslateFile(input, AsmPath(input)), StatusIs(absl::StatusCode::kNotFound));
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
