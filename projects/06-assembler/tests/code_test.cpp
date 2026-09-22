#include "code.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

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

TEST_P(CompCodeTableTest, Encodes) { EXPECT_EQ(GetParam().bits, comp_code(GetParam().mnemonic)); }

INSTANTIATE_TEST_SUITE_P(AllMnemonics, CompCodeTableTest, testing::ValuesIn(kCompCases));

TEST(CompCodeTest, ThrowsOnInvalidComp) { EXPECT_THROW(comp_code("D*2"), std::invalid_argument); }

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

TEST_P(DestCodeTableTest, Encodes) { EXPECT_EQ(GetParam().bits, dest_code(GetParam().mnemonic)); }

INSTANTIATE_TEST_SUITE_P(AllMnemonics, DestCodeTableTest, testing::ValuesIn(kDestCases));

TEST(DestCodeTest, ThrowsOnInvalidDest) { EXPECT_THROW(dest_code("XYZ"), std::invalid_argument); }

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

TEST_P(JumpCodeTableTest, Encodes) { EXPECT_EQ(GetParam().bits, jump_code(GetParam().mnemonic)); }

INSTANTIATE_TEST_SUITE_P(AllMnemonics, JumpCodeTableTest, testing::ValuesIn(kJumpCases));

TEST(JumpCodeTest, ThrowsOnInvalidJump) {
  EXPECT_THROW(jump_code("INVALID"), std::invalid_argument);
}

}  // namespace
