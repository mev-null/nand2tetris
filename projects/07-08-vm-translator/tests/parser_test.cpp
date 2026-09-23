#include "parser.hpp"

#include <optional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "absl/status/status.h"
#include "absl/status/status_matchers.h"
#include "absl/status/statusor.h"
#include "command.hpp"

namespace hack::vm {
namespace {

using ::absl_testing::IsOk;
using ::absl_testing::StatusIs;
using ::testing::HasSubstr;

// preprocess
TEST(ProcessLineTest, ReturnsCommandUnchanged) { EXPECT_EQ("add", ProcessLine("add")); }

TEST(ProcessLineTest, ReturnsCommandWithArgsUnchanged) {
  EXPECT_EQ("push local 3", ProcessLine("push local 3"));
}

TEST(ProcessLineTest, ReturnsEmptyStringForCommentOnlyLine) {
  EXPECT_EQ("", ProcessLine("// comment"));
}

TEST(ProcessLineTest, ReturnsEmptyStringForWhitespaceOnlyLine) {
  EXPECT_EQ("", ProcessLine("     "));
}

TEST(ProcessLineTest, ReturnsEmptyStringForEmptyLine) { EXPECT_EQ("", ProcessLine("")); }

TEST(ProcessLineTest, RemovesComment) { EXPECT_EQ("add", ProcessLine("add // comment")); }

TEST(ProcessLineTest, RemovesCommentWithoutSpace) { EXPECT_EQ("add", ProcessLine("add//comment")); }

TEST(ProcessLineTest, RemovesCommentAfterCommandWithArgs) {
  EXPECT_EQ("push local 3", ProcessLine("push local 3 // comment"));
}

TEST(ProcessLineTest, RemovesLeadingSpaces) { EXPECT_EQ("add", ProcessLine("    add")); }

TEST(ProcessLineTest, RemovesTrailingSpaces) { EXPECT_EQ("add", ProcessLine("add    ")); }

TEST(ProcessLineTest, RemovesLeadingAndTrailingSpace) {
  EXPECT_EQ("add", ProcessLine("    add    "));
}

TEST(ProcessLineTest, RemovesTab) { EXPECT_EQ("add", ProcessLine("\tadd\t")); }

TEST(ProcessLineTest, RemovesMixedWhitespace) { EXPECT_EQ("add", ProcessLine(" \t  add \t ")); }

TEST(ProcessLineTest, RemovesSpaceAndComment) {
  EXPECT_EQ("add", ProcessLine("    add // comment"));
}

// CRLF line endings: std::getline leaves the '\r' at the end of each line
TEST(ProcessLineTest, RemovesCarriageReturn) { EXPECT_EQ("add", ProcessLine("add\r")); }

TEST(ProcessLineTest, ReturnsEmptyStringForCarriageReturnOnlyLine) {
  EXPECT_EQ("", ProcessLine("\r"));
}

TEST(ProcessLineTest, RemovesSpaceAndCarriageReturn) {
  EXPECT_EQ("push local 3", ProcessLine("  push local 3  \r"));
}

// parse
TEST(ParseCommandTest, ParsesPushWithSegmentAndIndex) {
  absl::StatusOr<Command> command = ParseCommand("push local 3");

  ASSERT_THAT(command, IsOk());
  EXPECT_EQ(CommandType::kPush, command->type);
  EXPECT_EQ("local", command->arg1);
  EXPECT_EQ(3, command->arg2);
}

TEST(ParseCommandTest, ParsesPopWithSegmentAndIndex) {
  absl::StatusOr<Command> command = ParseCommand("pop local 3");

  ASSERT_THAT(command, IsOk());
  EXPECT_EQ(CommandType::kPop, command->type);
  EXPECT_EQ("local", command->arg1);
  EXPECT_EQ(3, command->arg2);
}

TEST(ParseCommandTest, ParsesAdd) {
  absl::StatusOr<Command> command = ParseCommand("add");

  ASSERT_THAT(command, IsOk());
  EXPECT_EQ(CommandType::kArithmetic, command->type);
  EXPECT_EQ("add", command->arg1);
  EXPECT_EQ(std::nullopt, command->arg2);
}

TEST(ParseCommandTest, ParsesPushWithZeroIndex) {
  absl::StatusOr<Command> command = ParseCommand("push local 0");

  ASSERT_THAT(command, IsOk());
  EXPECT_EQ(CommandType::kPush, command->type);
  EXPECT_EQ("local", command->arg1);
  EXPECT_EQ(0, command->arg2);
}

TEST(ParseCommandTest, RejectPushWithoutSegmentAndIndex) {
  EXPECT_THAT(ParseCommand("push"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPushWithSegmentWithoutIndex) {
  EXPECT_THAT(ParseCommand("push local"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPushWithThreeArgs) {
  EXPECT_THAT(ParseCommand("push local 3 2"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPushWithStringIndex) {
  EXPECT_THAT(ParseCommand("push local abc"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPushWithTrailingCharactersAfterIndex) {
  EXPECT_THAT(ParseCommand("push local 3abc"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPushWithDecimalIndex) {
  EXPECT_THAT(ParseCommand("push local 3."), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPushWithSignedIndex) {
  EXPECT_THAT(ParseCommand("push local +3"), StatusIs(absl::StatusCode::kInvalidArgument));
}

// The two-argument StatusIs also matches the message.
TEST(ParseCommandTest, RejectPushWithNegativeIndex) {
  EXPECT_THAT(ParseCommand("push local -1"),
              StatusIs(absl::StatusCode::kInvalidArgument, HasSubstr("non-negative index")));
}

TEST(ParseCommandTest, RejectPushWithIndexOutOfRange) {
  EXPECT_THAT(ParseCommand("push local 99999999999"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPopWithoutSegmentAndIndex) {
  EXPECT_THAT(ParseCommand("pop"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPopWithSegmentWithoutIndex) {
  EXPECT_THAT(ParseCommand("pop local"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectPopWithThreeArgs) {
  EXPECT_THAT(ParseCommand("pop local 3 2"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectAddWithSegment) {
  EXPECT_THAT(ParseCommand("add local"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectAddWithSegmentAndIndex) {
  EXPECT_THAT(ParseCommand("add local 3"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectEmptyLine) {
  EXPECT_THAT(ParseCommand(""), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectUppercasedCommand) {
  EXPECT_THAT(ParseCommand("Push local 3"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectUnexpectedCommand) {
  EXPECT_THAT(ParseCommand("addi"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectUnexpectedCommandWithSegment) {
  EXPECT_THAT(ParseCommand("addi local"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectUnexpectedCommandWithSegmentAndIndex) {
  EXPECT_THAT(ParseCommand("addi local 3"), StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(ParseCommandTest, RejectUnexpectedSegment) {
  EXPECT_THAT(ParseCommand("push Local 1"), StatusIs(absl::StatusCode::kInvalidArgument));
}

}  // namespace
}  // namespace hack::vm
