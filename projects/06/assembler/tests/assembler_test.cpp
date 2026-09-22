#include "assembler.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

TEST(AInstructionEncoderTest, EncodesZero) {
  EXPECT_EQ("0000000000000000", encode_a_instruction("@0"));
}

TEST(AInstructionEncoderTest, EncodesOne) {
  EXPECT_EQ("0000000000000001", encode_a_instruction("@1"));
}

TEST(AInstructionEncoderTest, EncodesTwentyOne) {
  EXPECT_EQ("0000000000010101", encode_a_instruction("@21"));
}

TEST(AInstructionEncoderTest, EncodesMaxValue) {
  EXPECT_EQ("0111111111111111", encode_a_instruction("@32767"));
}

TEST(AInstructionEncoderTest, EncodesHighestBit) {
  EXPECT_EQ("0100000000000000", encode_a_instruction("@16384"));
}

TEST(AInstructionEncoderTest, ThrowsOnValueOutOfRange) {
  EXPECT_THROW(encode_a_instruction("@32768"), std::invalid_argument);
}

TEST(AInstructionEncoderTest, ThrowsOnNegativeValue) {
  EXPECT_THROW(encode_a_instruction("@-1"), std::invalid_argument);
}

namespace {

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
  EXPECT_EQ(GetParam().binary, encode_c_instruction(GetParam().instruction));
}

INSTANTIATE_TEST_SUITE_P(RepresentativeInstructions, CInstructionEncoderTest,
                         testing::ValuesIn(kCInstructionCases));

TEST(CInstructionEncoderErrorTest, ThrowsOnInvalidComp) {
  EXPECT_THROW(encode_c_instruction("D=D*2"), std::invalid_argument);
}

TEST(CInstructionEncoderErrorTest, ThrowsOnInvalidDest) {
  EXPECT_THROW(encode_c_instruction("X=D"), std::invalid_argument);
}

TEST(CInstructionEncoderErrorTest, ThrowsOnInvalidJump) {
  EXPECT_THROW(encode_c_instruction("0;JMPX"), std::invalid_argument);
}

TEST(CInstructionEncoderErrorTest, ThrowsOnMissingComp) {
  EXPECT_THROW(encode_c_instruction("D="), std::invalid_argument);
}

}  // namespace
