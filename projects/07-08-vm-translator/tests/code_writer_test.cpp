#include "code_writer.hpp"

#include <sstream>

#include <gtest/gtest.h>

namespace hack::vm {
namespace {

TEST(CodeWriterTest, WritesSourceComment) {
  std::ostringstream output;
  CodeWriter writer(output, "Foo");

  writer.WriteSourceComment("push constant 7");
  EXPECT_EQ(output.str(), "// push constant 7\n");
}

}  // namespace
}  // namespace hack::vm
