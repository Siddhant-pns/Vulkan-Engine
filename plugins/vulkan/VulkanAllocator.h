#pragma once
#include "vk_mem_alloc.h"
#include "VulkanDevice.h"

namespace gfx {

class VulkanAllocator {
public:
    bool init(const backend::VulkanDevice& dev) {
        VmaAllocatorCreateInfo ci{};
        ci.physicalDevice = dev.physical();
        ci.device         = dev.logical();
        ci.instance       = dev.instance();
        return vmaCreateAllocator(&ci, &m_alloc) == VK_SUCCESS;
    }
    ~VulkanAllocator() { if (m_alloc) vmaDestroyAllocator(m_alloc); }

    VmaAllocator raw() const { return m_alloc; }

private:
    VmaAllocator m_alloc{};
};

} // namespace gfx
