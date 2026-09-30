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

Command Push(Segment segment, int index) {
  return {.type = CommandType::kPush,
          .op = std::nullopt,
          .arg1 = std::nullopt,
          .segment = segment,
          .arg2 = index};
}

Command Pop(Segment segment, int index) {
  return {.type = CommandType::kPop,
          .op = std::nullopt,
          .arg1 = std::nullopt,
          .segment = segment,
          .arg2 = index};
}

TEST(CodeWriterTest, WritesPushConstant) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kConstant, 7)), IsOk());
  EXPECT_EQ(output.str(),
            "@7\n"
            "D=A\n"
            "@SP\n"
            "A=M\n"
            "M=D\n"
            "@SP\n"
            "M=M+1\n");
}

TEST(CodeWriterTest, RejectPopConstant) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kConstant, 7)),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, WritesPushLocal) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kLocal, 7)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kLocal, 7)), IsOk());
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

TEST(CodeWriterTest, WritesPushArgument) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kArgument, 2)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kArgument, 2)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kThis, 2)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kThis, 2)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kThat, 2)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kThat, 2)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kTemp, 3)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kTemp, 3)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kTemp, 8)),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, RejectPopTempOutOfRange) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kTemp, 8)),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, WritesPushPointer0) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kPointer, 0)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kPointer, 0)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kPointer, 1)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kPointer, 1)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kPointer, 2)),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, RejectPopPointerOutOfRange) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kPointer, 2)),
              StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(CodeWriterTest, WritesPushStatic) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  EXPECT_THAT(writer.WriteCommand(Push(Segment::kStatic, 3)), IsOk());
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

  EXPECT_THAT(writer.WriteCommand(Pop(Segment::kStatic, 3)), IsOk());
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
