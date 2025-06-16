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
include_directories(${INCLUDE_DIR}/stb) 

# GLFW
add_subdirectory(external/glfw)
include_directories(external/glfw/include)

# Include Assimp
include_directories(${CMAKE_SOURCE_DIR}/external/assimp/include)
link_directories(${CMAKE_SOURCE_DIR}/external/assimp/lib)

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

add_custom_command(TARGET VulkanEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/external/assimp/bin/assimp-vc143-mt.dll
    $<TARGET_FILE_DIR:VulkanEngine>
)

target_link_libraries(VulkanEngine PRIVATE assimp-vc143-mt glfw volk Vulkan::Vulkan)
