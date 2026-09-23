#include "parser.hpp"

#include <optional>
#include <stdexcept>

#include <gtest/gtest.h>

#include "command.hpp"

namespace hack::vm {
namespace {

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
  Command command = ParseCommand("push local 3");

  EXPECT_EQ(CommandType::kPush, command.type);
  EXPECT_EQ("local", command.arg1);
  EXPECT_EQ(3, command.arg2);
}

TEST(ParseCommandTest, ParsesPopWithSegmentAndIndex) {
  Command command = ParseCommand("pop local 3");

  EXPECT_EQ(CommandType::kPop, command.type);
  EXPECT_EQ("local", command.arg1);
  EXPECT_EQ(3, command.arg2);
}

TEST(ParseCommandTest, ParsesAdd) {
  Command command = ParseCommand("add");

  EXPECT_EQ(CommandType::kArithmetic, command.type);
  EXPECT_EQ("add", command.arg1);
  EXPECT_EQ(std::nullopt, command.arg2);
}

TEST(ParseCommandTest, ParsesPushWithZeroIndex) {
  Command command = ParseCommand("push local 0");

  EXPECT_EQ(CommandType::kPush, command.type);
  EXPECT_EQ("local", command.arg1);
  EXPECT_EQ(0, command.arg2);
}

TEST(ParseCommandTest, RejectPushWithoutSegmentAndIndex) {
  EXPECT_THROW(ParseCommand("push");, std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithSegmentWithoutIndex) {
  EXPECT_THROW(ParseCommand("push local");, std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithThreeArgs) {
  EXPECT_THROW(ParseCommand("push local 3 2"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithStringIndex) {
  EXPECT_THROW(ParseCommand("push local abc"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithTrailingCharactersAfterIndex) {
  EXPECT_THROW(ParseCommand("push local 3abc"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithDecimalIndex) {
  EXPECT_THROW(ParseCommand("push local 3."), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithSignedIndex) {
  EXPECT_THROW(ParseCommand("push local +3"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithNegativeIndex) {
  EXPECT_THROW(ParseCommand("push local -1"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPushWithIndexOutOfRange) {
  EXPECT_THROW(ParseCommand("push local 99999999999"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPopWithoutSegmentAndIndex) {
  EXPECT_THROW(ParseCommand("pop"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPopWithSegmentWithoutIndex) {
  EXPECT_THROW(ParseCommand("pop local"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectPopWithThreeArgs) {
  EXPECT_THROW(ParseCommand("pop local 3 2"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectAddWithSegment) {
  EXPECT_THROW(ParseCommand("add local");, std::invalid_argument);
}

TEST(ParseCommandTest, RejectAddWithSegmentAndIndex) {
  EXPECT_THROW(ParseCommand("add local 3");, std::invalid_argument);
}

TEST(ParseCommandTest, RejectEmptyLine) { EXPECT_THROW(ParseCommand(""), std::invalid_argument); }

TEST(ParseCommandTest, RejectUppercasedCommand) {
  EXPECT_THROW(ParseCommand("Push local 3"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectUnexpectedCommand) {
  EXPECT_THROW(ParseCommand("addi"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectUnexpectedCommandWithSegment) {
  EXPECT_THROW(ParseCommand("addi local"), std::invalid_argument);
}

TEST(ParseCommandTest, RejectUnexpectedCommandWithSegmentAndIndex) {
  EXPECT_THROW(ParseCommand("addi local 3"), std::invalid_argument);
}

}  // namespace
}  // namespace hack::vm
