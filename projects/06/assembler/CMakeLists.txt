add_library(
    assembler_core
    src/parser.cpp
    src/code.cpp
    src/assembler.cpp
)

target_include_directories(
    assembler_core
    PUBLIC include
)

add_executable(
    hack_assembler
    src/main.cpp
)

target_link_libraries(
    hack_assembler
    PRIVATE assembler_core
)

add_executable(
    assembler_tests
    tests/parser_test.cpp
    tests/code_test.cpp
    tests/assembler_test.cpp
)

target_link_libraries(
    assembler_tests
    PRIVATE
    assembler_core
    GTest::gtest_main
)

gtest_discover_tests(assembler_tests)
