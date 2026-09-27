#include "code_writer.hpp"

#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/status/status.h"
#include "absl/status/status_matchers.h"
#include "command.hpp"

using ::absl_testing::IsOk;
using ::absl_testing::StatusIs;

namespace hack::vm {
namespace {

TEST(CodeWriterTest, WritesPushConstant) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kConstant,
                                   .arg2 = 7}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@7\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPushLocal) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kLocal,
                                   .arg2 = 7}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@LCL\n"
            "D=M\n"
            "@7\n"
            "D=D+A\n"
            "A=D\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopLocal) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kLocal,
                                   .arg2 = 7}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[LCL + 7] <- RAM[SP-1]
            // SP <- SP-1
            "@LCL\n"
            "D=M\n"
            "@7\n"
            "D=D+A\n"
            "@R13\n"
            "M=D\n"
            "@SP\n"
            "M=M-1\n"
            "A=M\n"
            "D=M\n"
            "@R13\n"
            "A=M\n"
            "M=D\n");
}

TEST(CodeWriterTest, RejectPopConstant) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kConstant,
                                   .arg2 = 7}),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, WritesAdd) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kAdd,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M+D\n");
}

TEST(CodeWriterTest, RejectArithmeticWithoutOp) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

}  // namespace
}  // namespace hack::vm
