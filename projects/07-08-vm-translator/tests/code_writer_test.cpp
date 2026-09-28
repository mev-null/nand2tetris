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

TEST(CodeWriterTest, WritesJEQ) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kEq,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
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

TEST(CodeWriterTest, WritesJGT) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kGt,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
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

TEST(CodeWriterTest, WritesJLT) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo1");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kLt,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
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
            "M=D+M\n");
}

TEST(CodeWriterTest, WritesSub) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kSub,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=M-D\n");
}

TEST(CodeWriterTest, WritesAnd) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kAnd,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
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

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kOr,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "M=D|M\n");
}

TEST(CodeWriterTest, WritesNeg) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kNeg,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "A=M-1\n"
            "M=-M\n");
}

TEST(CodeWriterTest, WritesNot) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kNot,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
  EXPECT_EQ(output.str(),
            "@SP\n"
            "A=M-1\n"
            "M=!M\n");
}

TEST(CodeWriterTest, WritesEq) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kEq,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
}

TEST(CodeWriterTest, WritesGt) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kGt,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
}

TEST(CodeWriterTest, WritesLt) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kArithmetic,
                                   .op = Operator::kLt,
                                   .arg1 = std::nullopt,
                                   .segment = std::nullopt,
                                   .arg2 = std::nullopt}),
              IsOk());
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
