#pragma once
#include "plugins/vulkan/VulkanBuffer.h"
#include "plugins/vulkan/VulkanDescriptor.h"
#include "plugins/vulkan/VulkanDevice.h"
#include "plugins/vulkan/VulkanPipeline.h"
#include "plugins/vulkan/VulkanShader.h"
#include "plugins/vulkan/VulkanSwapchain.h"
#include "plugins/vulkan/VulkanTexture.h"
#include "plugins/vulkan/VulkanUniformBuffer.h"

struct CubeResources {
    backend::VulkanBuffer vb, ib;
    backend::VulkanTexture tex;
    backend::VulkanDescriptor desc;
    backend::VulkanUniformBuffer ubo;
    backend::VulkanPipeline pipe;
    glm::mat4 mvp;
}; // forward
extern CubeResources cube; // global for demo

bool InitCube(const backend::VulkanDevice& dev, VkCommandPool pool, VkQueue graphicsQ,
              const backend::VulkanSwapchain& swap);
void DestroyCube(const backend::VulkanDevice& dev);
