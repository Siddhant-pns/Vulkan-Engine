# extern/vma/CMakeLists.txt
add_library(vma INTERFACE)
target_include_directories(vma INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
