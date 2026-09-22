#include "symbol_table.hpp"

#include <gtest/gtest.h>

TEST(SymbolTableTest, AddsAndFindsSymbol) {
  SymbolTable table_;

  table_.AddEntry("LOOP", 10);

  EXPECT_TRUE(table_.Contains("LOOP"));
  EXPECT_EQ(10, table_.GetAddress("LOOP"));
}

TEST(Symboltable_Test, ReturnsFalseForUnknownSymbol) {
  SymbolTable table_;

  EXPECT_FALSE(table_.Contains("UNKNOWN"));
}

TEST(Symboltable_Test, ThrowsForUnknownSymbol) {
  SymbolTable table_;

  EXPECT_THROW(table_.GetAddress("UNKNOWN"), std::invalid_argument);
}
