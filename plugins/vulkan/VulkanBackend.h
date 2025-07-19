// VulkanBackend.h
#pragma once
#include "VulkanAllocator.h" // for memory management
#include "VulkanCommand.h"   // for command buffer management
#include "VulkanDevice.h"    // existing helper
#include "VulkanInstance.h"
#include "VulkanPipeline.h"
#include "VulkanSwapchain.h" // for swapchain management
#include "VulkanSync.h"      // for synchronization primitives
#include "VulkanUtils.h"
#include "backend/include/IRenderBackend.h"
#include "core/util/Logger.h"
#include "glm/glm.hpp"
#include <memory>
#include <unordered_map>

namespace gfx {

class VulkanBackend final : public IRenderBackend {
  public:
    ~VulkanBackend() noexcept override = default;
    bool init(void* windowHandle) override;
    gfx::CmdHandle beginFrame() override;
    void endFrame(gfx::CmdHandle) override;
    void clearColor(gfx::CmdHandle, gfx::TextureHandle, const float[4]) override;

    void shutdown() override;
    void preShutdown() override;

    TextureHandle createTexture(const TextureDesc&) override;
    BufferHandle createBuffer(const BufferDesc&) override;
    void destroyTexture(TextureHandle) override;
    void destroyBuffer(BufferHandle) override;

    VkCommandBuffer currentCommandBuffer() const noexcept {
        return m_currentCmd;
    }

    VkImage currentImage() const {
        return m_swapchain.CurrentImage(m_frameIndex);
    }
    const std::vector<backend::VulkanSync>& GetSyncObjects() const {
        return m_sync;
    }

    /* --- thin immediate helpers (implementation in .cpp) --- */
    void cmdBeginRenderPass(gfx::CmdHandle, void* pipeVoid, uint32_t fbIdx);
    void cmdEndRenderPass(gfx::CmdHandle);
    void cmdBindPipeline(gfx::CmdHandle, void* pipePtr);
    void cmdBindDescriptorSets(gfx::CmdHandle, void* layoutPtr, void* set0Ptr, void* set1Ptr);
    void cmdBindVertexBuffer(gfx::CmdHandle, void* bufPtr);
    void cmdBindIndexBuffer(gfx::CmdHandle, void* bufPtr, int indexType);
    void cmdPushConstants(gfx::CmdHandle, void* layoutPtr, const glm::mat4&);
    void cmdDrawIndexed(gfx::CmdHandle, uint32_t idxCnt, uint32_t instCnt, uint32_t firstIdx, int32_t vtxOffset,
                        uint32_t firstInst);

    backend::VulkanDevice& device() {
        return m_device;
    }
    backend::VulkanCommand& commands() {
        return m_cmd;
    }
    backend::VulkanSwapchain& swapchain() {
        return m_swap;
    }
    VkQueue graphicsQ() {
        return m_device.GetGraphicsQueue();
    }

    void transition(gfx::CmdHandle cmd, gfx::TextureHandle tex, int oldLayout, int newLayout, int srcAccess,
                    int dstAccess, int srcStage, int dstStage) override;

  private:
    uint32_t m_nextHandle = 1;
    /* simple maps; replace with pool later */
    std::unordered_map<uint32_t, VkImage> m_images;
    std::unordered_map<uint32_t, VkBuffer> m_buffers;

    VulkanAllocator m_allocator{};
    VkCommandPool m_cmdPool{};
    backend::VulkanSwapchain m_swapchain;
    std::vector<VkImage> m_swapImages;
    std::vector<VkImageView> m_swapViews;
    uint32_t m_curImage = 0;

    backend::VulkanInstance m_instanceMgr;
    backend::VulkanDevice m_device;
    VkSurfaceKHR m_surface{};

    backend::VulkanSwapchain m_swap;         // ← reuse
    backend::VulkanCommand m_cmd;            // ← reuse
    std::vector<backend::VulkanSync> m_sync; // ← reuse

    gfx::VulkanAllocator m_alloc;

    uint32_t m_frameIndex = 0; // rotates 0..N-1

    VkCommandBuffer m_currentCmd{VK_NULL_HANDLE};
    uint32_t m_currentImg{0};
    std::vector<VkImageLayout> m_imgLayout; // init after swapchain create

    VkExtent2D extent;
};

IRenderBackend* createVulkanBackend(); // factory

} // namespace gfx
