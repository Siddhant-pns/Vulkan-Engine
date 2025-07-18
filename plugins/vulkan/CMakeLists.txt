cmake_minimum_required(VERSION 3.16)
project(vulkan_backend LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Vulkan backend sources
# ------------------------------------------------------------------------------
add_library(vulkan_backend STATIC
    VulkanAllocator.cpp
    VulkanBackend.cpp
    VulkanDevice.cpp
    VulkanInstance.cpp
    VulkanUtils.cpp
    VulkanSwapchain.cpp
    VulkanCommand.cpp
    VulkanSync.cpp
    VulkanPipeline.cpp
    VulkanDescriptor.cpp
    VulkanUniformBuffer.cpp
    VulkanTexture.cpp
    VulkanShader.cpp
    VulkanBuffer.cpp
    VulkanTexture.cpp
)

# ------------------------------------------------------------------------------
# Public include dirs (engine headers + extern stuff)
# ------------------------------------------------------------------------------
target_include_directories(vulkan_backend
    PUBLIC
        ../../src                  # backend/include/...
        ../../extern/vma           # vk_mem_alloc.h
)

# ------------------------------------------------------------------------------
# Link libraries
#   * volk  – single static lib defined in root CMakeLists
#   * Vulkan::Vulkan – loader import lib from SDK (needed by volk at link-time)
#   * glfw  – window/surface creation
#   * vma   – if you made VMA its own static library target
# ------------------------------------------------------------------------------
find_package(Vulkan REQUIRED)      # gives Vulkan::Vulkan

target_link_libraries(vulkan_backend
    PUBLIC  backend_iface core volk stb
    PRIVATE Vulkan::Vulkan glfw vma
)

# ------------------------------------------------------------------------------
# Compile definitions
# Volk already defines VK_NO_PROTOTYPES for itself; just inherit it.
# ------------------------------------------------------------------------------
