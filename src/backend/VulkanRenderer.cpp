#include "VulkanRenderer.h"
#include <volk.h>
#include <iostream>

namespace backend {

void VulkanRenderer::Init(void* windowPtr) {
    std::cout << "[VulkanRenderer] Initializing Vulkan Renderer...\n";
    window = static_cast<platform::Window*>(windowPtr); 

    instanceManager.Create(true);
    surface = window->CreateVulkanSurface(instanceManager.Get());
    deviceManager.Create(instanceManager.Get(), surface);
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = 0;

    vkCreateFence(deviceManager.GetLogical(), &fenceInfo, nullptr, &imageAvailableFence);
    swapchainManager.Create(deviceManager.GetPhysical(), deviceManager.GetLogical(), surface);

    // Create render pass and framebuffer
    pipelineManager.CreateRenderPass(deviceManager.GetLogical(), swapchainManager.GetFormat());
    pipelineManager.CreateFramebuffers(deviceManager.GetLogical(), swapchainManager.GetExtent(), swapchainManager.GetImageViews());
    commandManager.Create(
    deviceManager.GetLogical(),
    /* queueFamilyIndex */ 0, // For now we assume graphics queue is index 0
    static_cast<uint32_t>(swapchainManager.GetImageViews().size())
  );
    vertexShader.LoadFromFile(deviceManager.GetLogical(), "shaders/basic.vert.spv");
    fragmentShader.LoadFromFile(deviceManager.GetLogical(), "shaders/basic.frag.spv");

    pipelineManager.CreateGraphicsPipeline(
        deviceManager.GetLogical(),
        swapchainManager.GetExtent(),
        pipelineManager.GetRenderPass(),
        vertexShader.Get(),
        fragmentShader.Get()
    );

}

void VulkanRenderer::BeginFrame() {
    VkDevice device = deviceManager.GetLogical();
    VkQueue graphicsQueue;
    vkGetDeviceQueue(device, 0, 0, &graphicsQueue); // assume graphics queue index = 0


   // Acquire next image
    VkResult result = vkAcquireNextImageKHR(
        device,
        swapchainManager.Get(),
        UINT64_MAX,
        VK_NULL_HANDLE,
        imageAvailableFence,
        &currentImageIndex
    );

    // Now wait until image is available (signaled by driver)
    vkWaitForFences(device, 1, &imageAvailableFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &imageAvailableFence);

    // Record command buffer
    VkCommandBuffer cmd = commandManager.GetBuffers()[currentImageIndex];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkClearValue clearColor = { {{0.1f, 0.1f, 0.3f, 1.0f}} };

    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = pipelineManager.GetRenderPass();
    rpInfo.framebuffer = pipelineManager.GetFramebuffers()[currentImageIndex];
    rpInfo.renderArea.offset = {0, 0};
    rpInfo.renderArea.extent = swapchainManager.GetExtent();
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.GetPipeline());
    vkCmdDraw(cmd, 3, 1, 0, 0);  // 3 vertices, 1 instance

    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    // Submit
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    // TEMPORARY: Wait for queue to finish processing commands    
    vkQueueWaitIdle(graphicsQueue); // TEMPORARY: sync per frame (later replaced with semaphores/fences)
}

void VulkanRenderer::EndFrame() {
    VkQueue graphicsQueue;
    vkGetDeviceQueue(deviceManager.GetLogical(), 0, 0, &graphicsQueue);

    // Present frame
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    VkSwapchainKHR swapchain = swapchainManager.Get();
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &currentImageIndex;

    vkQueuePresentKHR(graphicsQueue, &presentInfo);

}


void VulkanRenderer::Cleanup() {

    if (imageAvailableFence != VK_NULL_HANDLE) {
    vkDestroyFence(deviceManager.GetLogical(), imageAvailableFence, nullptr);
    imageAvailableFence = VK_NULL_HANDLE;
    }

    pipelineManager.Destroy(deviceManager.GetLogical());
    swapchainManager.Destroy();
    commandManager.Destroy(deviceManager.GetLogical());
    deviceManager.Destroy(); 

    if (surface != VK_NULL_HANDLE) {
    vkDestroySurfaceKHR(instanceManager.Get(), surface, nullptr);
    surface = VK_NULL_HANDLE;
    }
    instanceManager.Destroy();
    std::cout << "[VulkanRenderer] Vulkan instance destroyed\n";
}

}
