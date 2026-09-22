add_executable(
    vm_translator
    src/main.cpp
)

# Acceptance: every course program of chapters 7 and 8, translated and run on the CPU
# Emulator. The *VME.tst scripts run the VM code on the VM Emulator and do not exercise
# the translator.
find_package(Python3 REQUIRED COMPONENTS Interpreter)

file(GLOB tests CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*.tst)
list(FILTER tests EXCLUDE REGEX "VME\\.tst$")
foreach(test IN LISTS tests)
    cmake_path(GET test STEM name)
    # e.g. StackArithmetic/SimpleAdd/SimpleAdd.tst -> Translate.StackArithmetic.SimpleAdd
    cmake_path(GET test PARENT_PATH program_dir)
    cmake_path(GET program_dir PARENT_PATH group_dir)
    cmake_path(GET group_dir FILENAME group)
    add_test(
        NAME Translate.${group}.${name}
        COMMAND Python3::Interpreter ${PROJECT_SOURCE_DIR}/scripts/run_vm_test.py
                $<TARGET_FILE:vm_translator> ${test}
    )
    set_tests_properties(Translate.${group}.${name} PROPERTIES LABELS "acceptance;vm")
endforeach()
