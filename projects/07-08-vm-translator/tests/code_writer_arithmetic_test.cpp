#include "code_writer.hpp"

#include <optional>
#include <sstream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/status/status.h"
#include "absl/status/status_matchers.h"
#include "command.hpp"

using ::absl_testing::IsOk;
using ::absl_testing::StatusIs;
using ::testing::HasSubstr;

namespace hack::vm {
namespace {

Command Arithmetic(Operator op) {
  return {.type = CommandType::kArithmetic,
          .op = op,
          .arg1 = std::nullopt,
          .segment = std::nullopt,
          .arg2 = std::nullopt};
}

TEST(CodeWriterTest, WritesAdd) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kAdd)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D+M\n");
}

TEST(CodeWriterTest, WritesSub) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kSub)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D\n");
}

TEST(CodeWriterTest, WritesNeg) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kNeg)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "A=M-1\n"
            "M=-M\n");
}

TEST(CodeWriterTest, WritesEq) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kEq)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo.CMP.0.TRUE\n"
            "D;JEQ\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "@Foo.CMP.0.END\n"
            "0;JMP\n"
            "(Foo.CMP.0.TRUE)\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "(Foo.CMP.0.END)\n");
}

TEST(CodeWriterTest, WritesGt) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kGt)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo.CMP.0.TRUE\n"
            "D;JGT\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "@Foo.CMP.0.END\n"
            "0;JMP\n"
            "(Foo.CMP.0.TRUE)\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "(Foo.CMP.0.END)\n");
}

TEST(CodeWriterTest, WritesLt) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo1");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kLt)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n"
            "@Foo1.CMP.0.TRUE\n"
            "D;JLT\n"
            "@SP\n"
            "A=M-1\n"
            "M=0\n"
            "@Foo1.CMP.0.END\n"
            "0;JMP\n"
            "(Foo1.CMP.0.TRUE)\n"
            "@SP\n"
            "A=M-1\n"
            "M=-1\n"
            "(Foo1.CMP.0.END)\n");
}

TEST(CodeWriterTest, WritesAnd) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kAnd)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D&M\n");
}

TEST(CodeWriterTest, WritesOr) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kOr)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M\n");
}

TEST(CodeWriterTest, WritesNot) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kNot)), IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "A=M-1\n"
            "M=!M\n");
}

TEST(CodeWriterTest, WritesUniqueLabelsForEachComparison) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kEq)), IsOk());
  EXPECT_THAT(writer.WriteCommand(Arithmetic(Operator::kEq)), IsOk());
  EXPECT_THAT(output.str(), HasSubstr("(Foo.CMP.0.TRUE)"));
  EXPECT_THAT(output.str(), HasSubstr("(Foo.CMP.1.TRUE)"));
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
