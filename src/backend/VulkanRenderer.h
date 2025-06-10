#pragma once

#include "renderer/IRenderer.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "VulkanShader.h"
#include "platform/Window.h"

namespace backend {
    class VulkanRenderer : public renderer::IRenderer {
    public:
        void Init(void* windowPtr) override;
        void BeginFrame() override;
        void EndFrame() override;
        void Cleanup() override;

    private:
        VulkanInstance instanceManager;
        VulkanDevice deviceManager;
        VulkanSwapchain swapchainManager;
        VulkanPipeline pipelineManager;
        VulkanCommand commandManager;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        VkFence imageAvailableFence = VK_NULL_HANDLE;

        uint32_t currentImageIndex = 0;

        platform::Window* window = nullptr; 

        VulkanShader vertexShader;
        VulkanShader fragmentShader;

    };
}
