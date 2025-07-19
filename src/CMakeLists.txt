# src/CMakeLists.txt

file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS
    *.cpp
    *.h
)

add_library(vulkan_engine STATIC ${ENGINE_SOURCES})

target_include_directories(vulkan_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vulkan_engine
    glfw
    volk
    vma
)

# optional: link Vulkan SDK if needed
find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_engine Vulkan::Vulkan)
