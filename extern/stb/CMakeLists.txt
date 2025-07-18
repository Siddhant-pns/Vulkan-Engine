# extern/stb/CMakeLists.txt
add_library(stb INTERFACE)

# Path that contains stb_image.h, stb_image_resize.h, stb_image_write.h ...
target_include_directories(stb INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}      # extern/stb/
)

# Nothing to compile; it's header-only
