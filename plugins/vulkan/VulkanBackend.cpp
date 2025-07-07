#include "VulkanBackend.h"
#include "platform/Window.h"
#include "core/Application.h"

// All Vulkan modules
#include "VulkanInstance.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "VulkanShader.h"
#include "VulkanDescriptor.h"
#include "VulkanUniformBuffer.h"
#include "VulkanTexture.h"
#include "resources/Mesh.h"
#include "scene/Camera.h"
#include "resources/UniformBuffer.h"

#include <volk.h>
#include <array>
#include <iostream>

class VulkanBackend::Impl {
public:
    backend::VulkanInstance instanceManager;
    backend::VulkanDevice deviceManager;
    backend::VulkanSwapchain swapchainManager;
    backend::VulkanPipeline pipelineManager;
    backend::VulkanCommand commandManager;
    backend::VulkanShader vertexShader, fragmentShader;
    backend::VulkanBuffer vertexBuffer;
    backend::VulkanDescriptor descriptor;
    backend::VulkanUniformBuffer uniformBuffer;
    backend::VulkanTexture texture;
    backend::Mesh mesh1, mesh2;
    std::vector<backend::Mesh> meshes;
    std::vector<glm::mat4> transforms;

    platform::Window window;
    scene::Camera camera;

    VkSurfaceKHR surface = VK_NULL_HANDLE;

    static constexpr int MAX_FRAMES_IN_FLIGHT = 3;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores{};
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores{};
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences{};
    uint32_t currentFrame = 0;
    uint32_t currentImageIndex = 0;

    void initialize() {
        window.Init();
        glm::uvec2 size = window.GetFramebufferSize();

        instanceManager.Create(true);
        surface = window.CreateVulkanSurface(instanceManager.Get());
        deviceManager.Create(instanceManager.Get(), surface);

        VkSemaphoreCreateInfo semInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
        VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            vkCreateSemaphore(deviceManager.GetLogical(), &semInfo, nullptr, &imageAvailableSemaphores[i]);
            vkCreateSemaphore(deviceManager.GetLogical(), &semInfo, nullptr, &renderFinishedSemaphores[i]);
            vkCreateFence(deviceManager.GetLogical(), &fenceInfo, nullptr, &inFlightFences[i]);
        }

        VkExtent2D extent = { size.x, size.y };
        camera.SetViewportSize(extent.width, extent.height);

        commandManager.Create(deviceManager.GetLogical(), 0, MAX_FRAMES_IN_FLIGHT);
        swapchainManager.Create(deviceManager.GetPhysical(), deviceManager.GetLogical(), surface, extent, commandManager.GetPool(), deviceManager.GetGraphicsQueue());

        pipelineManager.CreateRenderPass(deviceManager.GetLogical(), swapchainManager.GetFormat(), swapchainManager.GetDepthFormat());
        pipelineManager.CreateFramebuffers(deviceManager.GetLogical(), swapchainManager.GetExtent(), swapchainManager.GetImageViews(), swapchainManager.GetDepthImageView());

        vertexShader.LoadFromFile(deviceManager.GetLogical(), "shaders/basic.vert.spv");
        fragmentShader.LoadFromFile(deviceManager.GetLogical(), "shaders/basic.frag.spv");

        descriptor.Create(deviceManager.GetLogical());
        descriptor.Allocate(deviceManager.GetLogical());

        mesh1.LoadFromFile("assets/model1.obj");
        mesh1.UploadToGPU(deviceManager.GetLogical(), deviceManager.GetPhysical(), commandManager.GetPool(), deviceManager.GetGraphicsQueue());
        mesh2.LoadFromFile("assets/model2.obj");
        mesh2.UploadToGPU(deviceManager.GetLogical(), deviceManager.GetPhysical(), commandManager.GetPool(), deviceManager.GetGraphicsQueue());
        meshes = { mesh1, mesh2 };

        transforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
        transforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(+1.0f, 0.0f, 0.0f)));

        texture.LoadFromFile(deviceManager.GetLogical(), deviceManager.GetPhysical(), commandManager.GetPool(), deviceManager.GetGraphicsQueue(), "assets/texture.png");
        descriptor.Update(deviceManager.GetLogical(), texture.GetImageView(), texture.GetSampler());

        uniformBuffer.Create(deviceManager.GetLogical(), deviceManager.GetPhysical(), sizeof(UniformBufferObject), MAX_FRAMES_IN_FLIGHT);
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
            descriptor.Update(deviceManager.GetLogical(), uniformBuffer.GetDescriptorInfo(i));

        pipelineManager.CreateGraphicsPipeline(
            deviceManager.GetLogical(),
            swapchainManager.GetExtent(),
            pipelineManager.GetRenderPass(),
            vertexShader.Get(),
            fragmentShader.Get(),
            descriptor.GetLayout0(),
            descriptor.GetLayout1()
        );
    }

    void drawFrame() {
        if (window.WasResized()) return;

        auto device = deviceManager.GetLogical();
        auto queue = deviceManager.GetGraphicsQueue();

        vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &inFlightFences[currentFrame]);

        vkAcquireNextImageKHR(device, swapchainManager.Get(), UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &currentImageIndex);

        UniformBufferObject ubo{};
        ubo.view = camera.GetViewMatrix();
        ubo.proj = camera.GetProjectionMatrix();
        ubo.proj[1][1] *= -1;
        uniformBuffer.Update(device, currentFrame, &ubo, sizeof(ubo));

        VkCommandBuffer cmd = commandManager.Get(currentImageIndex);

        VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        vkBeginCommandBuffer(cmd, &beginInfo);

        std::array<VkClearValue, 2> clears{};
        clears[0].color = { 0.05f, 0.05f, 0.1f, 1.0f };
        clears[1].depthStencil = { 1.0f, 0 };

        VkRenderPassBeginInfo rpInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        rpInfo.renderPass = pipelineManager.GetRenderPass();
        rpInfo.framebuffer = pipelineManager.GetFramebuffers()[currentImageIndex];
        rpInfo.renderArea.extent = swapchainManager.GetExtent();
        rpInfo.clearValueCount = static_cast<uint32_t>(clears.size());
        rpInfo.pClearValues = clears.data();

        vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.GetPipeline());

        for (size_t i = 0; i < meshes.size(); ++i) {
            VkDescriptorSet sets[] = { descriptor.GetSet0(), descriptor.GetSet1() };
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.GetPipelineLayout(), 0, 2, sets, 0, nullptr);

            VkBuffer vb[] = { meshes[i].GetVertexBuffer() };
            VkDeviceSize offset[] = { 0 };
            vkCmdBindVertexBuffers(cmd, 0, 1, vb, offset);
            vkCmdBindIndexBuffer(cmd, meshes[i].GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdPushConstants(cmd, pipelineManager.GetPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &transforms[i]);
            vkCmdDrawIndexed(cmd, meshes[i].GetIndexCount(), 1, 0, 0, 0);
        }

        vkCmdEndRenderPass(cmd);
        vkEndCommandBuffer(cmd);

        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSubmitInfo submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
        submit.pWaitDstStageMask = waitStages;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

        vkQueueSubmit(queue, 1, &submit, inFlightFences[currentFrame]);

        VkPresentInfoKHR present{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
        present.swapchainCount = 1;
        VkSwapchainKHR swp = swapchainManager.Get();
        present.pSwapchains = &swp;
        present.pImageIndices = &currentImageIndex;
        vkQueuePresentKHR(queue, &present);

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    void shutdown() {
        deviceManager.Destroy();
        instanceManager.Destroy();
    }
};

// -- plugin interface --
VulkanBackend::VulkanBackend() : pImpl(std::make_unique<Impl>()) {}
VulkanBackend::~VulkanBackend() = default;

void VulkanBackend::initialize() { pImpl->initialize(); }
void VulkanBackend::drawFrame()  { pImpl->drawFrame(); }
void VulkanBackend::shutdown()   { pImpl->shutdown(); }

extern "C" re::IRenderBackend* createPlugin() {
    return new VulkanBackend();
}

extern "C" void destroyPlugin(re::IRenderBackend* p) {
    delete p;
}
