#include "parser.hpp"

#include <stdexcept>

#include <gtest/gtest.h>

#include "instruction.hpp"

// preprocess
TEST(ProcessLineTest, ReturnsInstructionUnchanged) { EXPECT_EQ("D=A", process_line("D=A")); }

TEST(ProcessLineTest, ReturnsEmptyStringForCommentOnlyLine) {
  EXPECT_EQ("", process_line("// comment"));
}

TEST(ProcessLineTest, ReturnsEmptyStringForWhitespaceOnlyLine) {
  EXPECT_EQ("", process_line("     "));
}

TEST(ProcessLineTest, RemovesComment) { EXPECT_EQ("D=A", process_line("D=A // comment")); }

TEST(ProcessLineTest, RemovesLeadingSpaces) { EXPECT_EQ("D=A", process_line("    D=A")); }

TEST(ProcessLineTest, RemovesTrailingSpaces) { EXPECT_EQ("D=A", process_line("D=A    ")); }

TEST(ProcessLineTest, RemovesLeadingAndTrailingSpace) {
  EXPECT_EQ("D=A", process_line("    D=A    "));
}

TEST(ProcessLineTest, RemovesTab) { EXPECT_EQ("D=A", process_line("\tD=A\t")); }

TEST(ProcessLineTest, RemovesMixedWhitespace) { EXPECT_EQ("D=A", process_line(" \t  D=A \t ")); }

TEST(ProcessLineTest, RemovesSpaceAndComment) {
  EXPECT_EQ("D=A", process_line("    D=A // comment"));
}

TEST(ProcessLineTest, ReturnsEmptyStringForEmptyLine) { EXPECT_EQ("", process_line("")); }

TEST(ProcessLineTest, RemovesCommentWithoutSpace) {
  EXPECT_EQ("D=A", process_line("D=A//comment"));
}

TEST(ProcessLineTest, RemovesCommentAfterLabel) {
  EXPECT_EQ("(LOOP)", process_line("(LOOP) // comment"));
}

// CRLF line endings: std::getline leaves the '\r' at the end of each line
TEST(ProcessLineTest, RemovesCarriageReturn) { EXPECT_EQ("D=A", process_line("D=A\r")); }

TEST(ProcessLineTest, ReturnsEmptyStringForCarriageReturnOnlyLine) {
  EXPECT_EQ("", process_line("\r"));
}

TEST(ProcessLineTest, RemovesSpaceAndCarriageReturn) { EXPECT_EQ("@2", process_line("  @2  \r")); }

// clasify instruction type
TEST(InstructionTypeTest, AInstruction) {
  InstructionType result = instruction_type("@12");

  EXPECT_EQ(InstructionType::A_INSTRUCTION, result);
}

TEST(InstructionTypeTest, SymbolicAInstruction) {
  InstructionType result = instruction_type("@LOOP");

  EXPECT_EQ(InstructionType::A_INSTRUCTION, result);
}

TEST(InstructionTypeTest, CInstructionComp) {
  InstructionType result = instruction_type("D");

  EXPECT_EQ(InstructionType::C_INSTRUCTION, result);
}

TEST(InstructionTypeTest, CInstructionDestAndComp) {
  InstructionType result = instruction_type("A=M");

  EXPECT_EQ(InstructionType::C_INSTRUCTION, result);
}

TEST(InstructionTypeTest, CInstructionCompAndJump) {
  InstructionType result = instruction_type("0;JUMP");

  EXPECT_EQ(InstructionType::C_INSTRUCTION, result);
}

TEST(InstructionTypeTest, CInstructionDestAndCompAndJump) {
  InstructionType result = instruction_type("D=D+A;JGT");

  EXPECT_EQ(InstructionType::C_INSTRUCTION, result);
}

TEST(InstructionTypeTest, LInstruction) {
  InstructionType result = instruction_type("(LOOP)");

  EXPECT_EQ(InstructionType::L_INSTRUCTION, result);
}

TEST(InstructionTypeTest, ThrowsOnEmptyInstruction) {
  EXPECT_THROW(instruction_type(""), std::invalid_argument);
}

// parse symbol
TEST(ParseSymbolTest, ParseAInstruction) { EXPECT_EQ("12", parse_symbol("@12")); }

TEST(ParseSymbolTest, ParseSymbolicAInstruction) { EXPECT_EQ("LOOP", parse_symbol("@LOOP")); }

TEST(ParseSymbolTest, ParseLInstruction) { EXPECT_EQ("LOOP", parse_symbol("(LOOP)")); }

TEST(ParseSymbolTest, ThrowsOnEmptyInstruction) {
  EXPECT_THROW(parse_symbol(""), std::invalid_argument);
}

TEST(ParseSymbolTest, ThrowsOnInvalidAInstruction) {
  EXPECT_THROW(parse_symbol("@"), std::invalid_argument);
}

TEST(ParseSymbolTest, ThrowsOnInvalidLInstruction) {
  EXPECT_THROW(parse_symbol("()"), std::invalid_argument);
}

TEST(ParseSymbolTest, ThrowsOnCInstruction) {
  EXPECT_THROW(parse_symbol("A=D"), std::invalid_argument);
}

// perse C instruction
TEST(CInstructionParserTest, ParsesDestAndComp) {
  CInstruction result = parse_c_instruction("D=M");

  EXPECT_EQ("D", result.dest);
  EXPECT_EQ("M", result.comp);
  EXPECT_EQ("", result.jump);
}

TEST(CInstructionParserTest, ParsesCompAndJump) {
  CInstruction result = parse_c_instruction("0;JMP");

  EXPECT_EQ("", result.dest);
  EXPECT_EQ("0", result.comp);
  EXPECT_EQ("JMP", result.jump);
}

TEST(CInstructionParserTest, ParsesDestAndCompAndJump) {
  CInstruction result = parse_c_instruction("D=D+A;JGT");

  EXPECT_EQ("D", result.dest);
  EXPECT_EQ("D+A", result.comp);
  EXPECT_EQ("JGT", result.jump);
}

TEST(CInstructionParserTest, ParsesComp) {
  CInstruction result = parse_c_instruction("D");

  EXPECT_EQ("", result.dest);
  EXPECT_EQ("D", result.comp);
  EXPECT_EQ("", result.jump);
}

TEST(CInstructionParserTest, ParsesDestAndCompWithOrAndJump) {
  CInstruction result = parse_c_instruction("AMD=D|A;JN");

  EXPECT_EQ("AMD", result.dest);
  EXPECT_EQ("D|A", result.comp);
  EXPECT_EQ("JN", result.jump);
}
