#pragma once
#include <volk.h>
#include <vector>

namespace backend {
    class VulkanCommand {
    public:
        void Create(VkDevice device, uint32_t queueFamilyIndex, uint32_t count);
        void Destroy(VkDevice device);

        const std::vector<VkCommandBuffer>& GetBuffers() const { return commandBuffers; }
        VkCommandPool GetPool() const { return commandPool; }

    private:
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
    };
}
