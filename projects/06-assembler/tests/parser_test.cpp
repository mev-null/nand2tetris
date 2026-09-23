#include "parser.hpp"

#include <stdexcept>

#include <gtest/gtest.h>

#include "instruction.hpp"

namespace hack::assembler {

// preprocess
TEST(ProcessLineTest, ReturnsInstructionUnchanged) { EXPECT_EQ("D=A", ProcessLine("D=A")); }

TEST(ProcessLineTest, ReturnsEmptyStringForCommentOnlyLine) {
  EXPECT_EQ("", ProcessLine("// comment"));
}

TEST(ProcessLineTest, ReturnsEmptyStringForWhitespaceOnlyLine) {
  EXPECT_EQ("", ProcessLine("     "));
}

TEST(ProcessLineTest, RemovesComment) { EXPECT_EQ("D=A", ProcessLine("D=A // comment")); }

TEST(ProcessLineTest, RemovesLeadingSpaces) { EXPECT_EQ("D=A", ProcessLine("    D=A")); }

TEST(ProcessLineTest, RemovesTrailingSpaces) { EXPECT_EQ("D=A", ProcessLine("D=A    ")); }

TEST(ProcessLineTest, RemovesLeadingAndTrailingSpace) {
  EXPECT_EQ("D=A", ProcessLine("    D=A    "));
}

TEST(ProcessLineTest, RemovesTab) { EXPECT_EQ("D=A", ProcessLine("\tD=A\t")); }

TEST(ProcessLineTest, RemovesMixedWhitespace) { EXPECT_EQ("D=A", ProcessLine(" \t  D=A \t ")); }

TEST(ProcessLineTest, RemovesSpaceAndComment) {
  EXPECT_EQ("D=A", ProcessLine("    D=A // comment"));
}

TEST(ProcessLineTest, ReturnsEmptyStringForEmptyLine) { EXPECT_EQ("", ProcessLine("")); }

TEST(ProcessLineTest, RemovesCommentWithoutSpace) { EXPECT_EQ("D=A", ProcessLine("D=A//comment")); }

TEST(ProcessLineTest, RemovesCommentAfterLabel) {
  EXPECT_EQ("(LOOP)", ProcessLine("(LOOP) // comment"));
}

// CRLF line endings: std::getline leaves the '\r' at the end of each line
TEST(ProcessLineTest, RemovesCarriageReturn) { EXPECT_EQ("D=A", ProcessLine("D=A\r")); }

TEST(ProcessLineTest, ReturnsEmptyStringForCarriageReturnOnlyLine) {
  EXPECT_EQ("", ProcessLine("\r"));
}

TEST(ProcessLineTest, RemovesSpaceAndCarriageReturn) { EXPECT_EQ("@2", ProcessLine("  @2  \r")); }

// clasify instruction type
TEST(InstructionTypeTest, AInstruction) {
  InstructionType result = ClassifyInstruction("@12");

  EXPECT_EQ(InstructionType::kAInstruction, result);
}

TEST(InstructionTypeTest, SymbolicAInstruction) {
  InstructionType result = ClassifyInstruction("@LOOP");

  EXPECT_EQ(InstructionType::kAInstruction, result);
}

TEST(InstructionTypeTest, CInstructionComp) {
  InstructionType result = ClassifyInstruction("D");

  EXPECT_EQ(InstructionType::kCInstruction, result);
}

TEST(InstructionTypeTest, CInstructionDestAndComp) {
  InstructionType result = ClassifyInstruction("A=M");

  EXPECT_EQ(InstructionType::kCInstruction, result);
}

TEST(InstructionTypeTest, CInstructionCompAndJump) {
  InstructionType result = ClassifyInstruction("0;JUMP");

  EXPECT_EQ(InstructionType::kCInstruction, result);
}

TEST(InstructionTypeTest, CInstructionDestAndCompAndJump) {
  InstructionType result = ClassifyInstruction("D=D+A;JGT");

  EXPECT_EQ(InstructionType::kCInstruction, result);
}

TEST(InstructionTypeTest, LInstruction) {
  InstructionType result = ClassifyInstruction("(LOOP)");

  EXPECT_EQ(InstructionType::kLInstruction, result);
}

TEST(InstructionTypeTest, ThrowsOnEmptyInstruction) {
  EXPECT_THROW(ClassifyInstruction(""), std::invalid_argument);
}

// parse symbol
TEST(ParseSymbolTest, ParseAInstruction) { EXPECT_EQ("12", ParseSymbol("@12")); }

TEST(ParseSymbolTest, ParseSymbolicAInstruction) { EXPECT_EQ("LOOP", ParseSymbol("@LOOP")); }

TEST(ParseSymbolTest, ParseLInstruction) { EXPECT_EQ("LOOP", ParseSymbol("(LOOP)")); }

TEST(ParseSymbolTest, ThrowsOnEmptyInstruction) {
  EXPECT_THROW(ParseSymbol(""), std::invalid_argument);
}

TEST(ParseSymbolTest, ThrowsOnInvalidAInstruction) {
  EXPECT_THROW(ParseSymbol("@"), std::invalid_argument);
}

TEST(ParseSymbolTest, ThrowsOnInvalidLInstruction) {
  EXPECT_THROW(ParseSymbol("()"), std::invalid_argument);
}

TEST(ParseSymbolTest, ThrowsOnCInstruction) {
  EXPECT_THROW(ParseSymbol("A=D"), std::invalid_argument);
}

// perse C instruction
TEST(CInstructionParserTest, ParsesDestAndComp) {
  CInstruction result = ParseCInstruction("D=M");

  EXPECT_EQ("D", result.dest);
  EXPECT_EQ("M", result.comp);
  EXPECT_EQ("", result.jump);
}

TEST(CInstructionParserTest, ParsesCompAndJump) {
  CInstruction result = ParseCInstruction("0;JMP");

  EXPECT_EQ("", result.dest);
  EXPECT_EQ("0", result.comp);
  EXPECT_EQ("JMP", result.jump);
}

TEST(CInstructionParserTest, ParsesDestAndCompAndJump) {
  CInstruction result = ParseCInstruction("D=D+A;JGT");

  EXPECT_EQ("D", result.dest);
  EXPECT_EQ("D+A", result.comp);
  EXPECT_EQ("JGT", result.jump);
}

TEST(CInstructionParserTest, ParsesComp) {
  CInstruction result = ParseCInstruction("D");

  EXPECT_EQ("", result.dest);
  EXPECT_EQ("D", result.comp);
  EXPECT_EQ("", result.jump);
}

TEST(CInstructionParserTest, ParsesDestAndCompWithOrAndJump) {
  CInstruction result = ParseCInstruction("AMD=D|A;JN");

  EXPECT_EQ("AMD", result.dest);
  EXPECT_EQ("D|A", result.comp);
  EXPECT_EQ("JN", result.jump);
}

}  // namespace hack::assembler
