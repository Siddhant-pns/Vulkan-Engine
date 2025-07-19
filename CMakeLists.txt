cmake_minimum_required(VERSION 3.23)

project(
    VulkanEngine
    VERSION 0.1.0
    LANGUAGES C CXX
)

# ---------------------------------------------------------------
# Options
# ---------------------------------------------------------------
option(VE_BUILD_TESTS       "Build Catch2 unit-tests"     ON)
option(VE_ENABLE_SANITIZERS "Enable Address/UBSan"        OFF)

# ---------------------------------------------------------------
# Global compile settings
# ---------------------------------------------------------------
set(CMAKE_CXX_STANDARD            20)
set(CMAKE_CXX_STANDARD_REQUIRED   ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ---------------------------------------------------------------
# External dependencies
# ---------------------------------------------------------------
add_subdirectory(extern/glfw)
add_subdirectory(extern/volk)
add_subdirectory(extern/vma)

# ---------------------------------------------------------------
# Engine targets
# ---------------------------------------------------------------
add_subdirectory(src)

if (VE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
