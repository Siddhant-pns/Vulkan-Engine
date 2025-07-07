#pragma once

#include "renderer/IRenderer.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"
#include "VulkanDescriptor.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture.h"
#include "resources/Mesh.h"
#include "scene/Camera.h"
#include "platform/Window.h"
#include "core/Application.h" // Ensure Application is included

namespace backend {
    class VulkanRenderer : public renderer::IRenderer {
    public:
        void Init(void* windowPtr) override;
        void BeginFrame() override;
        void RecreateSwapchain();
        void EndFrame() override;
        void Cleanup() override;

        void SetApplication(core::Application* app) { application = app; } 

    private:
        VulkanInstance instanceManager;
        VulkanDevice deviceManager;
        VulkanSwapchain swapchainManager;
        VulkanPipeline pipelineManager;
        VulkanCommand commandManager;
        VkSurfaceKHR surface = VK_NULL_HANDLE;

        void RecordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex);
        void RecordCommandBuffers();

        // VkFence imageAvailableFence = VK_NULL_HANDLE;

        uint32_t currentImageIndex = 0;

        static constexpr int MAX_FRAMES_IN_FLIGHT = 3;

        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores{};
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores{};
        std::array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences{};
        uint32_t currentFrame = 0;

        platform::Window* window = nullptr; 

        VulkanShader vertexShader;
        VulkanShader fragmentShader;
        VulkanBuffer vertexBuffer;
        VulkanDescriptor descriptor;
        VulkanUniformBuffer uniformBuffer;

        Mesh mesh1, mesh2;  // Declare as a member so it's accessible in BeginFrame()
        VulkanTexture texture;

        std::vector<Mesh> meshes;
        std::vector<glm::mat4> transforms; // per-mesh model matrices

        scene::Camera camera;
        core::Application* application = nullptr;


    };
}
