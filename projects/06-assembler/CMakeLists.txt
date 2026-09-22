add_library(
    assembler_core
    src/parser.cpp
    src/code.cpp
    src/assembler.cpp
    src/symbol_table.cpp
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
    tests/symbol_table_test.cpp
)

target_link_libraries(
    assembler_tests
    PRIVATE
    assembler_core
    GTest::gtest_main
)

gtest_discover_tests(assembler_tests)

# Acceptance: every course program, compared with the official Assembler
find_package(Python3 REQUIRED COMPONENTS Interpreter)

file(GLOB programs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/course/*/*.asm)
foreach(program IN LISTS programs)
    cmake_path(GET program STEM name)
    add_test(
        NAME Assemble.${name}
        COMMAND Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/compare_hack.py
                $<TARGET_FILE:hack_assembler> ${program}
    )
    set_tests_properties(Assemble.${name} PROPERTIES LABELS "acceptance;assembler")
endforeach()
