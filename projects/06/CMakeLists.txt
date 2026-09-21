add_subdirectory(assembler)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

file(GLOB programs CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*/*.asm)
foreach(program IN LISTS programs)
    cmake_path(GET program STEM name)
    add_test(
        NAME Assemble.${name}
        COMMAND Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/compare_hack.py
                $<TARGET_FILE:hack_assembler> ${program}
    )
    set_tests_properties(Assemble.${name} PROPERTIES LABELS acceptance)
endforeach()
