# tests/CMakeLists.txt

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


file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    *.cpp
    *.h
)

add_executable(unit_tests ${TEST_SOURCES})

target_include_directories(unit_tests PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(unit_tests
    vulkan_engine
    Catch2::Catch2WithMain
)
