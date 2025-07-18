#pragma once
#include <volk.h>
#include <vector>

namespace backend {
    class VulkanCommand {
    public:
        void Create(VkDevice device, uint32_t queueFamilyIndex, uint32_t count);
        void Destroy(VkDevice device);

        VkCommandBuffer Get(uint32_t index) const;
        uint32_t GetCount() const { return static_cast<uint32_t>(commandBuffers.size()); }
        VkCommandPool GetPool() const { return commandPool; }
        void Reset(VkDevice device);

    private:
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
    };
}
