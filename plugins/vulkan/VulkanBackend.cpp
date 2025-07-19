// VulkanBackend.cpp
#define VK_NO_PROTOTYPES
#include "VulkanBackend.h"
#include "core/util/Logger.h"
#include "platform/Window.h"
#include <glm/glm.hpp>
#include <iostream>
#include <volk.h>

using gfx::CmdHandle;

static VkCommandBuffer toCmd(CmdHandle h) {
    return static_cast<VkCommandBuffer>(h.ptr);
}

namespace gfx {

bool VulkanBackend::init(void* windowHandle) {
    auto* win = static_cast<core::platform::Window*>(windowHandle);

    /* 1. Create VkInstance (validation on for now) ------------------ */
    m_instanceMgr.Create(/*enableValidation=*/true);

    /* 2. Make the VkSurface from the GLFW window ------------------- */
    VkSurfaceKHR surface = win->CreateVulkanSurface(m_instanceMgr.Get());
    if (surface == VK_NULL_HANDLE) {
        core::util::Logger::error("[VK] surface creation failed");
        return false;
    }
    m_surface = surface;

    /* 3. Pick physical GPU + create logical device ----------------- */
    m_device.Create(m_instanceMgr.Get(), surface);

    m_swap.Create(m_device.physical(), m_device.logical(), surface, {1280, 720},
                  m_cmd.GetPool(), // not needed yet
                  m_device.GetGraphicsQueue());

    extent = m_swap.GetExtent();
    m_imgLayout.assign(m_swap.GetImageViews().size(), VK_IMAGE_LAYOUT_UNDEFINED);

    m_cmd.Create(m_device.logical(), m_device.graphicsFamily(), static_cast<uint32_t>(m_swap.GetImageViews().size()));
    m_sync.resize(m_swap.GetImageViews().size());
    for (auto& s : m_sync)
        s.Create(m_device.logical()); // fill TODOs in VulkanSync.cpp

    core::util::Logger::info("[VK] instance + device ready (swap-chain TBD)");
    return true; // ← success! the main loop will now stay alive
}

gfx::CmdHandle VulkanBackend::beginFrame() {
    /* ----- acquire & reset sync ----- */
    auto& s = m_sync[m_frameIndex];
    VkSemaphore ws = s.getImageAvailable();
    VkFence inf = s.getInFlight();
    vkWaitForFences(m_device.logical(), 1, &inf, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.logical(), 1, &inf);

    uint32_t imgIdx;
    vkAcquireNextImageKHR(m_device.logical(), m_swap.Get(), UINT64_MAX, s.getImageAvailable(), VK_NULL_HANDLE, &imgIdx);

    /* ----- start command buffer ----- */
    m_currentCmd = m_cmd.Get(imgIdx);
    m_currentImg = imgIdx;

    VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_currentCmd, &bi);

    /* ----- ensure image is COLOR_ATTACHMENT_OPTIMAL ----- */
    VkImageLayout old = m_imgLayout[m_currentImg];

    // Transition from TRANSFER_DST_OPTIMAL → COLOR_ATTACHMENT_OPTIMAL
    VkImageMemoryBarrier toClear{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    toClear.oldLayout = m_imgLayout[m_currentImg];
    toClear.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toClear.srcAccessMask = 0;
    toClear.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    toClear.image = m_swap.CurrentImage(m_currentImg);
    toClear.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdPipelineBarrier(m_currentCmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr,
                         0, nullptr, 1, &toClear);

    m_imgLayout[m_currentImg] = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

    return {m_currentCmd};
}

void VulkanBackend::endFrame(gfx::CmdHandle /*h*/) {
    auto& f = m_sync[m_frameIndex];
    VkSemaphore ws = f.getImageAvailable();
    VkSemaphore rf = f.getRenderFinished();
    VkFence inf = f.getInFlight();

    vkEndCommandBuffer(m_currentCmd);

    /* ----- submit + present ----- */
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    submit.waitSemaphoreCount = 1;
    submit.pWaitSemaphores = &ws;
    submit.pWaitDstStageMask = &waitStage;
    submit.commandBufferCount = 1;
    submit.pCommandBuffers = &m_currentCmd;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores = &rf;

    vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submit, f.getInFlight());

    VkPresentInfoKHR pres{VK_STRUCTURE_TYPE_PRESENT_INFO_KHR};
    pres.waitSemaphoreCount = 1;
    pres.pWaitSemaphores = &rf;
    pres.swapchainCount = 1;
    pres.pSwapchains = &m_swap.GetHandle();
    pres.pImageIndices = &m_currentImg;

    vkQueuePresentKHR(m_device.GetGraphicsQueue(), &pres);

    /* ----- update tracking & advance frame ----- */
    m_imgLayout[m_currentImg] = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    m_frameIndex = (m_frameIndex + 1) % m_sync.size();
}

void VulkanBackend::clearColor(gfx::CmdHandle h, gfx::TextureHandle, const float rgba[4]) {
    VkCommandBuffer cmd = static_cast<VkCommandBuffer>(h.ptr);
    VkImage img = m_swap.CurrentImage(m_currentImg);

    VkClearColorValue mag{{rgba[0], rgba[1], rgba[2], rgba[3]}};
    VkImageSubresourceRange sr{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdClearColorImage(cmd, img,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // ← layout
                         &mag, 1, &sr);
}

void VulkanBackend::transition(gfx::CmdHandle cmd, gfx::TextureHandle tex, int oldLayout, int newLayout, int srcAccess,
                               int dstAccess, int srcStage, int dstStage) {
    VkCommandBuffer vkCmd = static_cast<VkCommandBuffer>(cmd.ptr);
    VkImage vkImage = m_swap.CurrentImage(m_currentImg);

    VkImageMemoryBarrier barrier{VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};
    barrier.oldLayout = static_cast<VkImageLayout>(oldLayout);
    barrier.newLayout = static_cast<VkImageLayout>(newLayout);
    barrier.srcAccessMask = static_cast<VkAccessFlags>(srcAccess);
    barrier.dstAccessMask = static_cast<VkAccessFlags>(dstAccess);
    barrier.image = vkImage;
    barrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    vkCmdPipelineBarrier(vkCmd, static_cast<VkPipelineStageFlags>(srcStage),
                         static_cast<VkPipelineStageFlags>(dstStage), 0, 0, nullptr, 0, nullptr, 1, &barrier);

    m_imgLayout[m_currentImg] = static_cast<VkImageLayout>(newLayout);
}

void VulkanBackend::preShutdown() {
    // This is called before shutdown() to allow for any pre-shutdown tasks.
    // Step 1: Wait for in-flight frames to finish
    for (auto& s : m_sync)
        vkWaitForFences(m_device.logical(), 1, &s.inFlight, VK_TRUE, UINT64_MAX);

    // Step 2: Reset command pools to release resource references
    m_cmd.Reset(m_device.logical());

    // Step 3: Wait for all device queues to be idle
    vkDeviceWaitIdle(m_device.logical());

    core::util::Logger::info("[VK] Pre-shutdown tasks completed.");
}

void VulkanBackend::shutdown() {
    if (!m_device.logical())
        return;

    /* 2. Destroy per-frame sync first (not in use now) ----------- */
    for (auto& s : m_sync)
        s.Destroy(m_device.logical());
    m_sync.clear();

    /* 3. Command buffers & pool ---------------------------------- */
    m_cmd.Destroy(m_device.logical()); // wrapper frees buffers
    // if you created an extra pool, destroy it here

    /* 4. Swap-chain & depth -------------------------------------- */
    m_swap.Destroy(); // frees image-views, depth

    /* 5. Device, surface, instance ------------------------------- */
    m_device.Destroy(); // vkDestroyDevice
    vkDestroySurfaceKHR(m_instanceMgr.Get(), m_surface, nullptr);
    m_instanceMgr.Destroy(); // vkDestroyInstance

    core::util::Logger::info("[VK] backend shut down");
}

TextureHandle VulkanBackend::createTexture(const TextureDesc& d) {
    /* allocate VkImage; use d.width/d.height/format … */
    VkImage img = VK_NULL_HANDLE; // TODO real creation
    uint32_t id = m_nextHandle++;
    m_images[id] = img;
    return {id};
}
BufferHandle VulkanBackend::createBuffer(const BufferDesc& d) {
    VkBuffer buf = VK_NULL_HANDLE; // TODO real creation
    uint32_t id = m_nextHandle++;
    m_buffers[id] = buf;
    return {id};
}
void VulkanBackend::destroyTexture(TextureHandle h) {
    m_images.erase(h.id);
}
void VulkanBackend::destroyBuffer(BufferHandle h) {
    m_buffers.erase(h.id);
}

void VulkanBackend::cmdBeginRenderPass(CmdHandle h, void* pipeVoid, uint32_t fbIdx) {
    auto* pipe = static_cast<backend::VulkanPipeline*>(pipeVoid);
    VkCommandBuffer cmd = toCmd(h);

    VkFramebuffer fb = pipe->GetFramebuffers()[fbIdx];
    // VkExtent2D     extent = pipe->GetExtent();

    /* Optional clear values (depth is ignored by our pass anyway) */
    VkClearValue clears[2]{};
    clears[0].color = {0.f, 0.f, 0.f, 1.f};
    clears[1].depthStencil = {1.f, 0};

    VkRenderPassBeginInfo rpbi{VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO};
    rpbi.renderPass = pipe->GetRenderPass();
    rpbi.framebuffer = fb;
    rpbi.renderArea.extent = extent;
    rpbi.clearValueCount = 2;
    rpbi.pClearValues = clears;

    vkCmdBeginRenderPass(cmd, &rpbi, VK_SUBPASS_CONTENTS_INLINE);
}
void VulkanBackend::cmdEndRenderPass(CmdHandle h) {
    vkCmdEndRenderPass(toCmd(h));
}

/* 2. pipeline & descriptor binding -----------------------------------------*/
void VulkanBackend::cmdBindPipeline(CmdHandle h, void* pipePtr) {
    VkPipeline p = reinterpret_cast<VkPipeline>(pipePtr);
    vkCmdBindPipeline(toCmd(h), VK_PIPELINE_BIND_POINT_GRAPHICS, p);
}
void VulkanBackend::cmdBindDescriptorSets(CmdHandle h, void* layoutPtr, void* set0Ptr, void* set1Ptr) {
    VkPipelineLayout layout = reinterpret_cast<VkPipelineLayout>(layoutPtr);
    VkDescriptorSet sets[2]{reinterpret_cast<VkDescriptorSet>(set0Ptr), reinterpret_cast<VkDescriptorSet>(set1Ptr)};
    vkCmdBindDescriptorSets(toCmd(h), VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 2, sets, 0, nullptr);
}

/* 3. vertex / index buffers -------------------------------------------------*/
void VulkanBackend::cmdBindVertexBuffer(CmdHandle h, void* bufPtr) {
    VkBuffer vb = reinterpret_cast<VkBuffer>(bufPtr);
    VkDeviceSize offs = 0;
    vkCmdBindVertexBuffers(toCmd(h), 0, 1, &vb, &offs);
}
void VulkanBackend::cmdBindIndexBuffer(CmdHandle h, void* bufPtr, int indexType) {
    VkBuffer ib = reinterpret_cast<VkBuffer>(bufPtr);
    VkIndexType type = static_cast<VkIndexType>(indexType);
    vkCmdBindIndexBuffer(toCmd(h), ib, 0, type);
}

/* 4. push constants (MVP) ---------------------------------------------------*/
void VulkanBackend::cmdPushConstants(CmdHandle h, void* layoutPtr, const glm::mat4& mvp) {
    VkPipelineLayout layout = reinterpret_cast<VkPipelineLayout>(layoutPtr);
    vkCmdPushConstants(toCmd(h), layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &mvp);
}

/* 5. draw call --------------------------------------------------------------*/
void VulkanBackend::cmdDrawIndexed(CmdHandle h, uint32_t idxCnt, uint32_t instCnt, uint32_t firstIdx, int32_t vtxOffset,
                                   uint32_t firstInst) {
    vkCmdDrawIndexed(toCmd(h), idxCnt, instCnt, firstIdx, vtxOffset, firstInst);
}

IRenderBackend* createVulkanBackend() {
    return new VulkanBackend();
}

} // namespace gfx
