#include "VulkanRenderer.h"
#include <volk.h>
#include <iostream>
#include "scene/Vertex.h"
#include <glm/gtc/matrix_transform.hpp>
#include "resources/UniformBuffer.h"


namespace backend {

void VulkanRenderer::Init(void* windowPtr) {
    std::cout << "[VulkanRenderer] Initializing Vulkan Renderer...\n";
    window = static_cast<platform::Window*>(windowPtr); 
    glm::uvec2 size = window->GetFramebufferSize();

    instanceManager.Create(true);
    surface = window->CreateVulkanSurface(instanceManager.Get());
    deviceManager.Create(instanceManager.Get(), surface);

    VkSemaphoreCreateInfo semaphoreInfo{ VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    VkFenceCreateInfo fenceInfo{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        vkCreateSemaphore(deviceManager.GetLogical(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]);
        vkCreateSemaphore(deviceManager.GetLogical(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]);
        vkCreateFence(deviceManager.GetLogical(), &fenceInfo, nullptr, &inFlightFences[i]);
    }

    VkExtent2D extent = { size.x, size.y };
    camera.SetViewportSize(extent.width, extent.height);

    swapchainManager.Create(deviceManager.GetPhysical(), deviceManager.GetLogical(), surface, extent,
    commandManager.GetPool(),              
    deviceManager.GetGraphicsQueue() );

    // Create render pass and framebuffer
    pipelineManager.CreateRenderPass(deviceManager.GetLogical(), swapchainManager.GetFormat(), 
    swapchainManager.GetDepthFormat());
    pipelineManager.CreateFramebuffers(deviceManager.GetLogical(), swapchainManager.GetExtent(), swapchainManager.GetImageViews(), swapchainManager.GetDepthImageView());
    commandManager.Create(
    deviceManager.GetLogical(),
    /* queueFamilyIndex */ 0, // For now we assume graphics queue is index 0
    static_cast<uint32_t>(swapchainManager.GetImageViews().size())
  );
    // RecordCommandBuffers();
    vertexShader.LoadFromFile(deviceManager.GetLogical(), "shaders/basic.vert.spv");
    fragmentShader.LoadFromFile(deviceManager.GetLogical(), "shaders/basic.frag.spv");

    descriptor.Create(deviceManager.GetLogical());
    descriptor.Allocate(deviceManager.GetLogical());
    
    // Load mesh
    mesh1.LoadFromFile("assets/model1.obj");
    mesh1.UploadToGPU(
    deviceManager.GetLogical(),
    deviceManager.GetPhysical(),
    commandManager.GetPool(),
    deviceManager.GetGraphicsQueue()
);
    mesh2.LoadFromFile("assets/model2.obj");
    mesh2.UploadToGPU(
    deviceManager.GetLogical(),
    deviceManager.GetPhysical(),
    commandManager.GetPool(),
    deviceManager.GetGraphicsQueue()
);
    meshes.push_back(mesh1);
    meshes.push_back(mesh2);

    transforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)));
    transforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(+1.0f, 0.0f, 0.0f)));

    texture.LoadFromFile(
    deviceManager.GetLogical(),
    deviceManager.GetPhysical(),
    commandManager.GetPool(),
    deviceManager.GetGraphicsQueue(),
    "assets/texture.png"  // Replace with your actual texture file
);

    // Update descriptor set 0 (texture)
    descriptor.Update(
        deviceManager.GetLogical(),
        texture.GetImageView(),
        texture.GetSampler()
    );

    // Init UBO
    uniformBuffer.Create(deviceManager.GetLogical(),deviceManager.GetPhysical(), sizeof(UniformBufferObject), MAX_FRAMES_IN_FLIGHT);

    // Update UBO descriptor
    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        descriptor.Update(deviceManager.GetLogical(), uniformBuffer.GetDescriptorInfo(i));
    }

    pipelineManager.CreateGraphicsPipeline(
        deviceManager.GetLogical(),
        swapchainManager.GetExtent(),
        pipelineManager.GetRenderPass(),
        vertexShader.Get(),
        fragmentShader.Get(),
        descriptor.GetLayout0(),
        descriptor.GetLayout1() // Pass the second layout for uniform buffer
    );
    RecordCommandBuffers();     
}

void VulkanRenderer::BeginFrame() {
    if (window->WasResized())            // ← the flag you already have
    {
        RecreateSwapchain();             // rebuild everything
        return;                          // skip this frame – next loop will draw OK
    }

    static float time = 0.0f;
    time += 0.016f; // ~60fps
    
    VkExtent2D extent = swapchainManager.GetExtent();
    VkDevice device = deviceManager.GetLogical();
    VkQueue graphicsQueue = deviceManager.GetGraphicsQueue();

    // Wait for this frame’s fence
    vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFences[currentFrame]);

    VkResult result = vkAcquireNextImageKHR(
        device,
        swapchainManager.Get(),
        UINT64_MAX,
        imageAvailableSemaphores[currentFrame],
        VK_NULL_HANDLE,
        &currentImageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        RecreateSwapchain();
        return;
    }

    // Record command buffer
    VkCommandBuffer cmd = commandManager.Get(currentImageIndex);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd, &beginInfo);

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.05f, 0.05f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};

    // Begin render pass
    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = pipelineManager.GetRenderPass();
    rpInfo.framebuffer = pipelineManager.GetFramebuffers()[currentImageIndex];
    rpInfo.renderArea.offset = {0, 0};
    rpInfo.renderArea.extent = swapchainManager.GetExtent();
    rpInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    rpInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.GetPipeline());

    const scene::Camera& cam = application->GetCamera();
    UniformBufferObject ubo{};
    // ubo.model = transforms[i];
    glm::mat4 model = glm::mat4(1.0f);
    ubo.view = cam.GetViewMatrix();
    ubo.proj = cam.GetProjectionMatrix();
    ubo.proj[1][1] *= -1;

    uniformBuffer.Update(deviceManager.GetLogical(), currentFrame, &ubo, sizeof(ubo));

    // Draw the meshes
    for (size_t i = 0; i < meshes.size(); ++i) {
        transforms[i] = model;
        // Bind descriptor sets (shared for now)
        VkDescriptorSet sets[] = {
            descriptor.GetSet0(),  // texture
            descriptor.GetSet1()   // shared UBO
        };

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineManager.GetPipelineLayout(),
            0, 2, sets,
            0, nullptr
        );

        VkBuffer vb[] = { meshes[i].GetVertexBuffer() };
        VkDeviceSize offset[] = { 0 };
        vkCmdBindVertexBuffers(cmd, 0, 1, vb, offset);
        vkCmdBindIndexBuffer(cmd, meshes[i].GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

        vkCmdPushConstants(
            cmd,
            pipelineManager.GetPipelineLayout(),
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(glm::mat4),
            &transforms[i]  // Push the model matrix for each mesh
        );

        vkCmdDrawIndexed(cmd, meshes[i].GetIndexCount(), 1, 0, 0, 0);
}

// vkCmdDraw(cmd, 3, 1, 0, 0);  // no vertex buffer at all

    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);

    // Submit the frame

    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmd;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);

}

void VulkanRenderer::RecordCommandBuffer(VkCommandBuffer cmd, uint32_t imageIndex) {
    VkExtent2D extent = swapchainManager.GetExtent();

    UniformBufferObject ubo{};
    ubo.view = camera.GetViewMatrix();
    ubo.proj = camera.GetProjectionMatrix();
    ubo.proj[1][1] *= -1; // Flip Y for Vulkan

    VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkClearValue clearColor{ { 0.1f, 0.1f, 0.3f, 1.0f } };
    VkClearValue depthClear{};
    depthClear.depthStencil = { 1.0f, 0 };

    std::array<VkClearValue, 2> clearValues = { clearColor, depthClear };

    VkRenderPassBeginInfo renderPassInfo{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    renderPassInfo.renderPass = pipelineManager.GetRenderPass();
    renderPassInfo.framebuffer = pipelineManager.GetFramebuffers()[imageIndex];
    renderPassInfo.renderArea.extent = extent;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineManager.GetPipeline());

    // ✳️ Optional: bind descriptor sets here
    // ✳️ Optional: bind vertex + index buffer
    // ✳️ Optional: vkCmdDrawIndexed(...)

    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);
}

void VulkanRenderer::RecreateSwapchain() {
    vkDeviceWaitIdle(deviceManager.GetLogical());

    swapchainManager.Destroy();
    pipelineManager.Destroy(deviceManager.GetLogical());
    commandManager.Destroy(deviceManager.GetLogical());

    glm::uvec2 size = window->GetFramebufferSize();
    VkExtent2D extent = { size.x, size.y };

    swapchainManager.Create(
        deviceManager.GetPhysical(),
        deviceManager.GetLogical(),
        surface,
        extent,
        commandManager.GetPool(),
        deviceManager.GetGraphicsQueue()
    );

    uint32_t imageCount = static_cast<uint32_t>(swapchainManager.GetImageViews().size());
    if (imageCount == 0) {
        std::cerr << "[Error] Swapchain returned 0 image views!\n";
        return;
    }

    commandManager.Create( deviceManager.GetLogical(), 0, imageCount);

    pipelineManager.CreateRenderPass(
        deviceManager.GetLogical(),
        swapchainManager.GetFormat(),
        swapchainManager.GetDepthFormat()
    );
    
    pipelineManager.CreateGraphicsPipeline(
        deviceManager.GetLogical(),
        swapchainManager.GetExtent(),
        pipelineManager.GetRenderPass(),
        vertexShader.Get(),
        fragmentShader.Get(),
        descriptor.GetLayout0(),
        descriptor.GetLayout1()
    );

    pipelineManager.CreateFramebuffers(
        deviceManager.GetLogical(),
        swapchainManager.GetExtent(),
        swapchainManager.GetImageViews(),
        swapchainManager.GetDepthImageView()
    );

    RecordCommandBuffers();

    currentImageIndex = UINT32_MAX;
}


void VulkanRenderer::EndFrame() {

    if ( currentImageIndex == UINT32_MAX )   // ✅ only guard you need
        return;
    VkQueue graphicsQueue;
    vkGetDeviceQueue(deviceManager.GetLogical(), 0, 0, &graphicsQueue);

    // Present frame
    VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
    presentInfo.swapchainCount = 1;
    VkSwapchainKHR swapchain = swapchainManager.Get();
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &currentImageIndex;

    vkQueuePresentKHR(deviceManager.GetGraphicsQueue(), &presentInfo);

    // 🔁 move to next frame
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderer::Cleanup() {
    vkDeviceWaitIdle( deviceManager.GetLogical() );   
    for (auto& m : meshes)
    m.Destroy(deviceManager.GetLogical());
    uniformBuffer.Destroy(deviceManager.GetLogical());
    descriptor.Destroy(deviceManager.GetLogical());
    texture.Destroy(deviceManager.GetLogical());
    vertexBuffer.Destroy(deviceManager.GetLogical());
    vertexShader.Destroy(deviceManager.GetLogical());
    fragmentShader.Destroy(deviceManager.GetLogical());
    
    for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
        if (imageAvailableSemaphores[i]) vkDestroySemaphore(deviceManager.GetLogical(), imageAvailableSemaphores[i], nullptr);
        if (renderFinishedSemaphores[i]) vkDestroySemaphore(deviceManager.GetLogical(), renderFinishedSemaphores[i], nullptr);
        if (inFlightFences[i]) vkDestroyFence(deviceManager.GetLogical(), inFlightFences[i], nullptr);
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

void VulkanRenderer::RecordCommandBuffers() {
    for (size_t i = 0; i < commandManager.GetCount(); ++i) {
        RecordCommandBuffer(commandManager.Get(i), static_cast<uint32_t>(i));
    }
}

}
