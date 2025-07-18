add_library(null_backend STATIC
    NullBackend.cpp
)

target_include_directories(null_backend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../..        # so it sees backend/include
)
target_link_libraries(null_backend PUBLIC core glm)
