// VulkanBackend.cpp
#define VK_NO_PROTOTYPES 
#include <volk.h> 
#include "VulkanBackend.h"
#include "platform/Window.h"
#include "core/util/Logger.h"
#include <iostream>

namespace gfx {

bool VulkanBackend::init(void* windowHandle)
{
    auto* win = static_cast<core::platform::Window*>(windowHandle);

    /* 1. Create VkInstance (validation on for now) ------------------ */
    m_instanceMgr.Create(/*enableValidation=*/true);

    /* 2. Make the VkSurface from the GLFW window ------------------- */
    VkSurfaceKHR surface = win->CreateVulkanSurface(m_instanceMgr.Get());
    if (surface == VK_NULL_HANDLE) {
        core::util::Logger::error("[VK] surface creation failed");
        return false;
    }

    /* 3. Pick physical GPU + create logical device ----------------- */
    m_device.Create(m_instanceMgr.Get(), surface);

    m_cmd.Create(m_device.logical(),
             m_device.graphicsFamily(),
             /*count*/3);                       // alloc primary buffers

    m_swap.Create(m_device.physical(), m_device.logical(),
              surface, {1280,720},
              m_cmd.GetPool(),   // not needed yet
              m_device.GetGraphicsQueue());
    m_cmd.Create(m_device.logical(),
             m_device.graphicsFamily(),
             static_cast<uint32_t>(m_swap.GetImageViews().size()));
    m_sync.resize(m_swap.GetImageViews().size());
    for (auto& s : m_sync)  s.Create(m_device.logical());   // fill TODOs in VulkanSync.cpp

    core::util::Logger::info("[VK] instance + device ready (swap-chain TBD)");
    return true;   // ← success! the main loop will now stay alive
}

CmdHandle VulkanBackend::beginFrame(){
    auto& s = m_sync[m_frameIndex];
    VkSemaphore ws = s.getImageAvailable();
    VkFence inf = s.getInFlight();
    vkWaitForFences(m_device.logical(), 1, &inf, VK_TRUE, UINT64_MAX);
    vkResetFences(m_device.logical(), 1, &inf);

    uint32_t imgIdx;
    vkAcquireNextImageKHR(m_device.logical(), m_swap.Get(), UINT64_MAX,
                        s.getImageAvailable(), VK_NULL_HANDLE, &imgIdx);

    m_currentCmd = m_cmd.Get(imgIdx);
    m_currentImg = imgIdx;
    VkCommandBufferBeginInfo beginInfo{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(m_currentCmd, &beginInfo);   // 1-time
    VkImageMemoryBarrier toDst{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    toDst.oldLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    toDst.newLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toDst.srcAccessMask = 0;
    toDst.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    toDst.image         = m_swap.CurrentImage(imgIdx);
    toDst.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0,1,0,1 };

    vkCmdPipelineBarrier(m_currentCmd,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        0, 0,nullptr, 0,nullptr, 1,&toDst);

    return { m_currentCmd }; 

 }
void VulkanBackend::endFrame(CmdHandle h) {
    auto& f = m_sync[m_frameIndex];
    VkSemaphore ws = f.getImageAvailable();
    VkSemaphore rf = f.getRenderFinished();
    VkFence inf = f.getInFlight();

    VkImageMemoryBarrier toPresent{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
    toPresent.oldLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    toPresent.newLayout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    toPresent.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    toPresent.dstAccessMask = 0;
    toPresent.image         = m_swap.CurrentImage(m_currentImg);
    toPresent.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0,1,0,1 };

    vkCmdPipelineBarrier(m_currentCmd,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        0, 0,nullptr, 0,nullptr, 1,&toPresent);

    vkEndCommandBuffer(m_currentCmd);

    // wait on imageAvailable, signal renderFinished
    /* submit */
    VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submit.waitSemaphoreCount   = 1;
    submit.pWaitSemaphores      = &ws;
    submit.pWaitDstStageMask    = &waitStage;
    submit.commandBufferCount   = 1;
    submit.pCommandBuffers      = &m_currentCmd;
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores    = &rf;

    vkQueueSubmit(m_device.GetGraphicsQueue(), 1, &submit, f.getInFlight());

    // wait on renderFinished, index=currentImg
    /* present */
    VkPresentInfoKHR pres{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    pres.waitSemaphoreCount = 1;
    pres.pWaitSemaphores    = &rf;
    pres.swapchainCount     = 1;
    pres.pSwapchains = &m_swap.GetHandle();
    pres.pImageIndices      = &m_currentImg;
    
    vkQueuePresentKHR(m_device.GetGraphicsQueue(), &pres);

    m_frameIndex = (m_frameIndex + 1) % m_sync.size();
    std::cout << "[VK] Frame " << m_frameIndex << " ended." << std::endl;
 }

 void VulkanBackend::clearColor(CmdHandle h,
                               gfx::TextureHandle,
                               const float rgba[4])
{
    VkCommandBuffer cmd = static_cast<VkCommandBuffer>(h.ptr);
    VkImage img = m_swap.CurrentImage(m_currentImg);

    VkClearColorValue mag{{rgba[0],rgba[1],rgba[2],rgba[3]}};
    VkImageSubresourceRange sr{VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1};

    vkCmdClearColorImage(cmd, img,
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,   // ← layout
                         &mag, 1, &sr);
}

void VulkanBackend::shutdown()
{ 
    m_cmd.Destroy(m_device.logical());
    m_swap.Destroy();
    for (auto& s : m_sync) s.Destroy(m_device.logical());
    m_device.Destroy();
    m_instanceMgr.Destroy();
}

TextureHandle VulkanBackend::createTexture(const TextureDesc& d) {
    /* allocate VkImage; use d.width/d.height/format … */
    VkImage img = VK_NULL_HANDLE;   // TODO real creation
    uint32_t id = m_nextHandle++;
    m_images[id] = img;
    return {id};
}
BufferHandle VulkanBackend::createBuffer(const BufferDesc& d) {
    VkBuffer buf = VK_NULL_HANDLE;  // TODO real creation
    uint32_t id = m_nextHandle++;
    m_buffers[id] = buf;
    return {id};
}
void VulkanBackend::destroyTexture(TextureHandle h) { m_images.erase(h.id); }
void VulkanBackend::destroyBuffer (BufferHandle h)  { m_buffers.erase(h.id); }

IRenderBackend* createVulkanBackend() { return new VulkanBackend(); }

} // namespace gfx
