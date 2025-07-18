cmake_minimum_required(VERSION 3.20)
project(UltimateGfx LANGUAGES CXX)

# C++20 everywhere
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(backend_iface INTERFACE)
target_include_directories(backend_iface INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/src          # gives backend/include/...
)

# Add Volk
find_package(Vulkan REQUIRED)    
add_library(volk STATIC extern/volk/volk_impl.cpp)
target_include_directories(volk PUBLIC
     extern/volk
     ${Vulkan_INCLUDE_DIRS}
)
target_compile_definitions(volk PUBLIC VK_NO_PROTOTYPES)
target_link_libraries(volk PUBLIC Vulkan::Vulkan) 


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

add_subdirectory(plugins/null)

add_subdirectory(plugins/vulkan)

add_subdirectory(extern/vma)

add_subdirectory(extern/glfw)

add_subdirectory(extern/stb)

add_subdirectory(extern/glm)

# --------------------------------------------------------------------------------
# Core static library
# --------------------------------------------------------------------------------
add_library(core STATIC
    src/core/util/Logger.cpp
    src/core/util/Time.cpp
    src/core/util/FileSystem.cpp
    src/core/jobs/JobSystem.cpp  
)

target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src        # so #include "core/util/Logger.h" works
)

target_include_directories(core PUBLIC
    ${CMAKE_SOURCE_DIR}/extern/vma/include
)

add_library(platform STATIC
    src/platform/Window.cpp
)
target_link_libraries(platform PUBLIC glfw volk)

# --------------------------------------------------------------------------------
# Sandbox3D demo executable
# --------------------------------------------------------------------------------
add_executable(Sandbox3D
    src/app/Sandbox3D/main.cpp
)
target_link_libraries(Sandbox3D PRIVATE core backend_iface null_backend vulkan_backend)

target_sources(Sandbox3D PRIVATE
    src/app/Sandbox3D/CubeSetup.cpp
)
target_include_directories(Sandbox3D PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app/Sandbox3D  # for CubeSetup.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/graphics/data  # for CubeVerts.hpp
)


# ------------------------------------------------------------------------------
add_library(render_device STATIC
    src/backend/RenderDevice.cpp
)
target_link_libraries(render_device PUBLIC backend_iface core glfw vulkan_backend null_backend)
target_include_directories(render_device PUBLIC src )
target_link_libraries(Sandbox3D PRIVATE render_device)


add_library(graphics_render STATIC
    src/graphics/render/RenderGraph.cpp
)
target_link_libraries(graphics_render PUBLIC core backend_iface render_device)
target_include_directories(graphics_render PUBLIC src)
target_link_libraries(Sandbox3D PRIVATE graphics_render)

target_link_libraries(Sandbox3D PRIVATE
    platform
    graphics_render
    render_device
)
# ------------------------------------------------------------------------------
# Unit-test executable
# ------------------------------------------------------------------------------
add_executable(unit_tests
    tests/test_math.cpp
    tests/test_util.cpp
    tests/test_jobs.cpp 
    tests/test_mat4.cpp 
    tests/test_mat4_inverse.cpp   # <-- new
    tests/test_simd.cpp  
    tests/test_mat3.cpp            # NEW
    tests/test_transform.cpp       # NEW
    tests/test_mat4_simd.cpp   
)
target_link_libraries(unit_tests PRIVATE core Catch2::Catch2WithMain)
add_test(NAME all_core_tests COMMAND unit_tests)

# --------------------------------------------------------------------------------
# Convenience
# --------------------------------------------------------------------------------
set_target_properties(Sandbox3D PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
