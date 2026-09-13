#include <gtest/gtest.h>

#include <string>
#include <vector>

TEST(Toolchain, StandardLibraryIsAvailable) {
  const std::vector<std::string> parts{"nand", "tetris"};
  EXPECT_EQ(parts.at(0) + "2" + parts.at(1), "nand2tetris");
}
