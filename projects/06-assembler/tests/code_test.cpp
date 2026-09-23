#include "code.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "parser.hpp"

namespace {

struct CodeCase {
  std::string mnemonic;
  std::string bits;
};

std::ostream& operator<<(std::ostream& os, const CodeCase& c) {
  return os << (c.mnemonic.empty() ? "null" : c.mnemonic) << " -> " << c.bits;
}

// comp
const CodeCase kCompCases[] = {
    {"0",   "0101010"},
    {"1",   "0111111"},
    {"-1",  "0111010"},
    {"D",   "0001100"},
    {"A",   "0110000"},
    {"!D",  "0001101"},
    {"!A",  "0110001"},
    {"-D",  "0001111"},
    {"-A",  "0110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"},
    {"D+A", "0000010"},
    {"D-A", "0010011"},
    {"A-D", "0000111"},
    {"D&A", "0000000"},
    {"D|A", "0010101"},
    {"M",   "1110000"},
    {"!M",  "1110001"},
    {"-M",  "1110011"},
    {"M+1", "1110111"},
    {"M-1", "1110010"},
    {"D+M", "1000010"},
    {"D-M", "1010011"},
    {"M-D", "1000111"},
    {"D&M", "1000000"},
    {"D|M", "1010101"},
};

class CompCodeTableTest : public testing::TestWithParam<CodeCase> {};

TEST_P(CompCodeTableTest, Encodes) { EXPECT_EQ(GetParam().bits, CompCode(GetParam().mnemonic)); }

INSTANTIATE_TEST_SUITE_P(AllMnemonics, CompCodeTableTest, testing::ValuesIn(kCompCases));

TEST(CompCodeTest, ThrowsOnInvalidComp) { EXPECT_THROW(CompCode("D*2"), std::invalid_argument); }

// dest
const CodeCase kDestCases[] = {
    {"",    "000"},
    {"M",   "001"},
    {"D",   "010"},
    {"MD",  "011"},
    {"A",   "100"},
    {"AM",  "101"},
    {"AD",  "110"},
    {"AMD", "111"},
};

class DestCodeTableTest : public testing::TestWithParam<CodeCase> {};

TEST_P(DestCodeTableTest, Encodes) { EXPECT_EQ(GetParam().bits, DestCode(GetParam().mnemonic)); }

INSTANTIATE_TEST_SUITE_P(AllMnemonics, DestCodeTableTest, testing::ValuesIn(kDestCases));

TEST(DestCodeTest, ThrowsOnInvalidDest) { EXPECT_THROW(DestCode("XYZ"), std::invalid_argument); }

// jump
const CodeCase kJumpCases[] = {
    {"",    "000"},
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"},
};

class JumpCodeTableTest : public testing::TestWithParam<CodeCase> {};

TEST_P(JumpCodeTableTest, Encodes) { EXPECT_EQ(GetParam().bits, JumpCode(GetParam().mnemonic)); }

INSTANTIATE_TEST_SUITE_P(AllMnemonics, JumpCodeTableTest, testing::ValuesIn(kJumpCases));

TEST(JumpCodeTest, ThrowsOnInvalidJump) {
  EXPECT_THROW(JumpCode("INVALID"), std::invalid_argument);
}

// A instruction encoder
TEST(AInstructionEncoderTest, EncodesZero) { EXPECT_EQ("0000000000000000", EncodeAInstruction(0)); }

TEST(AInstructionEncoderTest, EncodesOne) { EXPECT_EQ("0000000000000001", EncodeAInstruction(1)); }

TEST(AInstructionEncoderTest, EncodesTwentyOne) {
  EXPECT_EQ("0000000000010101", EncodeAInstruction(21));
}

TEST(AInstructionEncoderTest, EncodesMaxValue) {
  EXPECT_EQ("0111111111111111", EncodeAInstruction(32767));
}

TEST(AInstructionEncoderTest, EncodesHighestBit) {
  EXPECT_EQ("0100000000000000", EncodeAInstruction(16384));
}

TEST(AInstructionEncoderTest, ThrowsOnValueOutOfRange) {
  EXPECT_THROW(EncodeAInstruction(32768), std::invalid_argument);
}

TEST(AInstructionEncoderTest, ThrowsOnNegativeValue) {
  EXPECT_THROW(EncodeAInstruction(-1), std::invalid_argument);
}

// C instruction encoder
struct CInstructionCase {
  std::string instruction;
  std::string binary;
};

std::ostream& operator<<(std::ostream& os, const CInstructionCase& c) {
  return os << c.instruction << " -> " << c.binary;
}

const CInstructionCase kCInstructionCases[] = {
    {"D=M",        "1111110000010000"},
    {"0;JMP",      "1110101010000111"},
    {"D=D+A",      "1110000010010000"},
    {"D=D+M;JGT",  "1111000010010001"},

    // every dest
    {"D=A",        "1110110000010000"},
    {"M=D",        "1110001100001000"},
    {"MD=M+1",     "1111110111011000"},
    {"A=M",        "1111110000100000"},
    {"AM=M-1",     "1111110010101000"},
    {"AD=D|A",     "1110010101110000"},
    {"AMD=D&M",    "1111000000111000"},

    // every jump
    {"D;JGT",      "1110001100000001"},
    {"D;JEQ",      "1110001100000010"},
    {"D;JGE",      "1110001100000011"},
    {"D;JLT",      "1110001100000100"},
    {"D;JNE",      "1110001100000101"},
    {"D;JLE",      "1110001100000110"},

    {"AM=M+1;JNE", "1111110111101101"},
    {"M=-1",       "1110111010001000"},
    {"D=!M",       "1111110001010000"},
};

class CInstructionEncoderTest : public testing::TestWithParam<CInstructionCase> {};

TEST_P(CInstructionEncoderTest, Encodes) {
  EXPECT_EQ(GetParam().binary, EncodeCInstruction(ParseCInstruction(GetParam().instruction)));
}

INSTANTIATE_TEST_SUITE_P(RepresentativeInstructions, CInstructionEncoderTest,
                         testing::ValuesIn(kCInstructionCases));

TEST(CInstructionEncoderErrorTest, ThrowsOnInvalidComp) {
  EXPECT_THROW(EncodeCInstruction(ParseCInstruction("D=D*2")), std::invalid_argument);
}

TEST(CInstructionEncoderErrorTest, ThrowsOnInvalidDest) {
  EXPECT_THROW(EncodeCInstruction(ParseCInstruction("X=D")), std::invalid_argument);
}

TEST(CInstructionEncoderErrorTest, ThrowsOnInvalidJump) {
  EXPECT_THROW(EncodeCInstruction(ParseCInstruction("0;JMPX")), std::invalid_argument);
}

TEST(CInstructionEncoderErrorTest, ThrowsOnMissingComp) {
  EXPECT_THROW(EncodeCInstruction(ParseCInstruction("D=")), std::invalid_argument);
}

}  // namespace
