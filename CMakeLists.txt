cmake_minimum_required(VERSION 3.24)
project(nand2tetris CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.18.0
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

set(ABSL_PROPAGATE_CXX_STD ON)
set(ABSL_ENABLE_INSTALL OFF)
set(ABSL_BUILD_TEST_HELPERS ON)
set(ABSL_USE_EXTERNAL_GOOGLETEST ON)
set(ABSL_FIND_GOOGLETEST OFF)
FetchContent_Declare(
  abseil
  GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
  GIT_TAG 20250814.2
)
FetchContent_MakeAvailable(abseil)

add_subdirectory(projects/06-assembler)
add_subdirectory(projects/07-08-vm-translator)
