cmake_minimum_required(VERSION 3.20)
project(UltimateGfx LANGUAGES CXX)

# C++20 everywhere
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Fetch Catch2 for unit tests
# ------------------------------------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.2           # or latest stable
)
FetchContent_MakeAvailable(catch2)

enable_testing()

# --------------------------------------------------------------------------------
# Core static library
# --------------------------------------------------------------------------------
add_library(core STATIC
    src/core/util/Logger.cpp
    src/core/util/Time.cpp
    src/core/util/FileSystem.cpp
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src        # so #include "core/util/Logger.h" works
)

# --------------------------------------------------------------------------------
# Sandbox3D demo executable
# --------------------------------------------------------------------------------
add_executable(Sandbox3D
    src/app/Sandbox3D/main.cpp
)
target_link_libraries(Sandbox3D PRIVATE core)

# ------------------------------------------------------------------------------
# Unit-test executable
# ------------------------------------------------------------------------------
add_executable(unit_tests
    tests/test_math.cpp
    tests/test_util.cpp
)
target_link_libraries(unit_tests PRIVATE core Catch2::Catch2WithMain)
add_test(NAME all_core_tests COMMAND unit_tests)

# --------------------------------------------------------------------------------
# Convenience
# --------------------------------------------------------------------------------
set_target_properties(Sandbox3D PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
