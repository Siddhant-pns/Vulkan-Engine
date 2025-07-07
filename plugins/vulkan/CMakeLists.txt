cmake_minimum_required(VERSION 3.16)
project(VulkanBackend LANGUAGES CXX)

# Enable shared library for plugin
add_library(vulkan_backend SHARED
    VulkanBackend.cpp
    VulkanBackend.h

    # Add all core Vulkan modules here
    VulkanBuffer.cpp  VulkanBuffer.h
    VulkanCommand.cpp VulkanCommand.h
    VulkanDescriptor.cpp VulkanDescriptor.h
    VulkanDevice.cpp VulkanDevice.h
    VulkanInstance.cpp VulkanInstance.h
    VulkanPipeline.cpp VulkanPipeline.h
    VulkanShader.cpp VulkanShader.h
    VulkanSwapchain.cpp VulkanSwapchain.h
    VulkanTexture.cpp VulkanTexture.h
    VulkanUniformBuffer.cpp VulkanUniformBuffer.h
    VulkanUtils.cpp VulkanUtils.h
)

# Define include paths
target_include_directories(vulkan_backend
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/engine
        ${CMAKE_SOURCE_DIR}/engine/platform
        ${CMAKE_SOURCE_DIR}/resources
        ${CMAKE_SOURCE_DIR}/scene
)

# Compile definitions
target_compile_definitions(vulkan_backend PRIVATE VK_NO_PROTOTYPES)

# Link external Vulkan + Volk
find_package(Vulkan REQUIRED)
target_link_libraries(vulkan_backend PRIVATE Vulkan::Vulkan)

# Optional: link Volk (if built separately)
# target_link_libraries(vulkan_backend PRIVATE volk)

# Set the output file name and suffix for platform
set_target_properties(vulkan_backend PROPERTIES
    OUTPUT_NAME "vulkan_backend"
    PREFIX ""                                # No "lib" prefix
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
