#pragma once
#define VK_NO_PROTOTYPES

#include <volk.h>
#include <cstdint>

namespace backend {
    class VulkanBuffer {
    public:
        void Create(VkDevice device, VkPhysicalDevice physicalDevice,
                    VkDeviceSize size, VkBufferUsageFlags usage,
                    VkMemoryPropertyFlags properties);

        void Upload(const void* data, VkDeviceSize size,
                    VkCommandPool pool, VkQueue graphicsQ);

        void Destroy(VkDevice device);

        VkBuffer Get() const { return buffer; }
        VkDeviceMemory GetMemory() const { return memory; }

    private:
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkDevice deviceRef = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDeviceRef = VK_NULL_HANDLE;
        VkDeviceSize bufferSize = 0;
        VkMemoryPropertyFlags memFlags{0};

        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };
}