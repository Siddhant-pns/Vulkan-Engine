cmake_minimum_required(VERSION 3.15)
project(VulkanEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set paths
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Add all source folders
include_directories(${SRC_DIR})
include_directories(${INCLUDE_DIR})

# GLFW
add_subdirectory(external/glfw)
include_directories(external/glfw/include)

# Vulan
add_subdirectory(external/volk)
include_directories(external/volk)

find_package(Vulkan REQUIRED)

# Collect source files
file(GLOB_RECURSE SRC_FILES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.h"
)

add_executable(VulkanEngine main.cpp ${SRC_FILES})

target_link_libraries(VulkanEngine PRIVATE glfw volk Vulkan::Vulkan)
