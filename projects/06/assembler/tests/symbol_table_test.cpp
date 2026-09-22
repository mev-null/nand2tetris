#include "symbol_table.hpp"

#include <ostream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

TEST(SymbolTableTest, AddsAndFindsSymbol) {
  SymbolTable table;

  table.AddEntry("LOOP", 10);

  EXPECT_TRUE(table.Contains("LOOP"));
  EXPECT_EQ(10, table.GetAddress("LOOP"));
}

TEST(SymbolTableTest, ReturnsFalseForUnknownSymbol) {
  SymbolTable table;

  EXPECT_FALSE(table.Contains("UNKNOWN"));
}

TEST(SymbolTableTest, ThrowsForUnknownSymbol) {
  SymbolTable table;

  EXPECT_THROW(table.GetAddress("UNKNOWN"), std::invalid_argument);
}

TEST(SymbolTableTest, ThrowsOnDuplicateSymbolAndKeepsFirstAddress) {
  SymbolTable table;
  table.AddEntry("LOOP", 10);

  EXPECT_THROW(table.AddEntry("LOOP", 20), std::invalid_argument);
  EXPECT_EQ(10, table.GetAddress("LOOP"));
}

TEST(SymbolTableTest, ThrowsOnPredefinedSymbol) {
  SymbolTable table;

  EXPECT_THROW(table.AddEntry("R0", 100), std::invalid_argument);
  EXPECT_EQ(0, table.GetAddress("R0"));
}

namespace {

struct PredefinedCase {
  std::string symbol;
  int address;
};

std::ostream& operator<<(std::ostream& os, const PredefinedCase& c) {
  return os << c.symbol << " -> " << c.address;
}

const PredefinedCase kPredefinedCases[] = {
    {"SP",     0    },
    {"LCL",    1    },
    {"ARG",    2    },
    {"THIS",   3    },
    {"THAT",   4    },
    {"R0",     0    },
    {"R1",     1    },
    {"R2",     2    },
    {"R3",     3    },
    {"R4",     4    },
    {"R5",     5    },
    {"R6",     6    },
    {"R7",     7    },
    {"R8",     8    },
    {"R9",     9    },
    {"R10",    10   },
    {"R11",    11   },
    {"R12",    12   },
    {"R13",    13   },
    {"R14",    14   },
    {"R15",    15   },
    {"SCREEN", 16384},
    {"KBD",    24576},
};

class PredefinedSymbolTest : public testing::TestWithParam<PredefinedCase> {};

TEST_P(PredefinedSymbolTest, HasAddress) {
  SymbolTable table;

  EXPECT_TRUE(table.Contains(GetParam().symbol));
  EXPECT_EQ(GetParam().address, table.GetAddress(GetParam().symbol));
}

INSTANTIATE_TEST_SUITE_P(AllSymbols, PredefinedSymbolTest, testing::ValuesIn(kPredefinedCases));

}  // namespace
