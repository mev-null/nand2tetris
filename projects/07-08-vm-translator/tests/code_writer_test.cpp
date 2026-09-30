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

TEST(CodeWriterTest, WritesPushArgument) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kArgument,
                                   .arg2 = 2}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- RAM[ARG + 2]
            // SP <- SP+1
            "@ARG\n"
            "D=M\n"
            "@2\n"
            "D=D+A\n"
            "A=D\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopArgument) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kArgument,
                                   .arg2 = 2}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[ARG + 2] <- RAM[SP-1]
            // SP <- SP-1
            "@ARG\n"
            "D=M\n"
            "@2\n"
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

TEST(CodeWriterTest, WritesPushThis) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kThis,
                                   .arg2 = 2}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- RAM[THIS + 2]
            // SP <- SP+1
            "@THIS\n"
            "D=M\n"
            "@2\n"
            "D=D+A\n"
            "A=D\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopThis) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kThis,
                                   .arg2 = 2}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[THIS + 2] <- RAM[SP-1]
            // SP <- SP-1
            "@THIS\n"
            "D=M\n"
            "@2\n"
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

TEST(CodeWriterTest, WritesPushThat) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kThat,
                                   .arg2 = 2}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- RAM[THAT + 2]
            // SP <- SP+1
            "@THAT\n"
            "D=M\n"
            "@2\n"
            "D=D+A\n"
            "A=D\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopThat) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kThat,
                                   .arg2 = 2}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[THAT + 2] <- RAM[SP-1]
            // SP <- SP-1
            "@THAT\n"
            "D=M\n"
            "@2\n"
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

TEST(CodeWriterTest, WritesPushTemp) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kTemp,
                                   .arg2 = 3}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- RAM[5 + 3]
            // SP <- SP+1
            "@R8\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopTemp) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kTemp,
                                   .arg2 = 3}),
              IsOk());
  EXPECT_EQ(output.str(),
            // SP <- SP-1
            // RAM[5 + 3] <- RAM[SP]
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@R8\n"
            "M=D\n");
}

TEST(CodeWriterTest, RejectPushTempOutOfRange) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kTemp,
                                   .arg2 = 8}),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, RejectPopTempOutOfRange) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kTemp,
                                   .arg2 = 8}),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, WritesPushPointer0) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kPointer,
                                   .arg2 = 0}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- THIS
            // SP <- SP+1
            "@THIS\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopPointer0) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kPointer,
                                   .arg2 = 0}),
              IsOk());
  EXPECT_EQ(output.str(),
            // SP <- SP-1
            // THIS <- RAM[SP]
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@THIS\n"
            "M=D\n");
}

TEST(CodeWriterTest, WritesPushPointer1) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kPointer,
                                   .arg2 = 1}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- THAT
            // SP <- SP+1
            "@THAT\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopPointer1) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kPointer,
                                   .arg2 = 1}),
              IsOk());
  EXPECT_EQ(output.str(),
            // SP <- SP-1
            // THAT <- RAM[SP]
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@THAT\n"
            "M=D\n");
}

TEST(CodeWriterTest, RejectPushPointerOutOfRange) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kPointer,
                                   .arg2 = 2}),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, RejectPopPointerOutOfRange) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kPointer,
                                   .arg2 = 2}),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, WritesPushStatic) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPush,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kStatic,
                                   .arg2 = 3}),
              IsOk());
  EXPECT_EQ(output.str(),
            // RAM[SP] <- Foo.3
            // SP <- SP+1
            "@Foo.3\n"
            "D=M\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, WritesPopStatic) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand({.type = CommandType::kPop,
                                   .op = std::nullopt,
                                   .arg1 = std::nullopt,
                                   .segment = Segment::kStatic,
                                   .arg2 = 3}),
              IsOk());
  EXPECT_EQ(output.str(),
            // SP <- SP-1
            // Foo.3 <- RAM[SP]
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "@Foo.3\n"
            "M=D\n");
}

}  // namespace
}  // namespace hack::vm
