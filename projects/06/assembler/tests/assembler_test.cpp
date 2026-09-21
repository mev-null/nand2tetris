#include "assembler.hpp"

#include <ostream>
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

TEST(AInstructionEncoderTest, ThrowsOnValueOutOfRange) {
  EXPECT_THROW(encode_a_instruction("@32768"), std::invalid_argument);
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
    {"D=M",       "1111110000010000"},
    {"0;JMP",     "1110101010000111"},
    {"D=D+A",     "1110000010010000"},
    {"D=D+M;JGT", "1111000010010001"},
};

class CInstructionEncoderTest : public testing::TestWithParam<CInstructionCase> {};

TEST_P(CInstructionEncoderTest, Encodes) {
  EXPECT_EQ(GetParam().binary, encode_c_instruction(GetParam().instruction));
}

INSTANTIATE_TEST_SUITE_P(RepresentativeInstructions, CInstructionEncoderTest,
                         testing::ValuesIn(kCInstructionCases));

}  // namespace
