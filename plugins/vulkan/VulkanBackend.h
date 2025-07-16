// VulkanBackend.h
#pragma once
#include <memory>  
#include "backend/include/IRenderBackend.h"
#include "core/util/Logger.h"
#include "VulkanDevice.h"   // existing helper
#include "VulkanUtils.h"
#include "VulkanInstance.h"
#include "VulkanSwapchain.h" // for swapchain management
#include "VulkanCommand.h"   // for command buffer management
#include "VulkanSync.h"      // for synchronization primitives
#include "VulkanAllocator.h"  // for memory management
#include <unordered_map>

namespace gfx {

class VulkanBackend final : public IRenderBackend {
public:
    ~VulkanBackend() noexcept override = default;
    bool init(void* windowHandle) override;
    CmdHandle beginFrame() override;
    void endFrame(CmdHandle) override;
    void clearColor(CmdHandle, gfx::TextureHandle, const float[4]) override;
    
    void shutdown() override;

    TextureHandle createTexture(const TextureDesc&) override;
    BufferHandle  createBuffer (const BufferDesc&) override;
    void destroyTexture(TextureHandle) override;
    void destroyBuffer (BufferHandle) override;

    VkCommandBuffer currentCommandBuffer() const noexcept { return m_currentCmd; }

    VkImage currentImage() const {
        return m_swapchain.CurrentImage(m_frameIndex);
    }

private:
    uint32_t      m_nextHandle = 1;
    /* simple maps; replace with pool later */
    std::unordered_map<uint32_t, VkImage>  m_images;
    std::unordered_map<uint32_t, VkBuffer> m_buffers;

    VulkanAllocator m_allocator;
    VkCommandPool   m_cmdPool{};
    backend::VulkanSwapchain m_swapchain;
    std::vector<VkImage>        m_swapImages;
    std::vector<VkImageView>    m_swapViews;
    uint32_t       m_curImage = 0;

    backend::VulkanInstance m_instanceMgr;
    // backend::VulkanDevice   m_deviceMgr;
    // backend::VulkanInstance m_instance;
    backend::VulkanDevice   m_device;

    backend::VulkanSwapchain m_swap;         // ← reuse
    backend::VulkanCommand   m_cmd;          // ← reuse
    std::vector<backend::VulkanSync> m_sync; // ← reuse

    gfx::VulkanAllocator   m_alloc;

    uint32_t m_frameIndex = 0;               // rotates 0..N-1

    VkCommandBuffer        m_currentCmd  {VK_NULL_HANDLE};
    uint32_t               m_currentImg  {0};

};

IRenderBackend* createVulkanBackend();      // factory

} // namespace gfx
