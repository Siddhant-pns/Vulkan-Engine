#pragma once
#include <volk.h>
#include <string>

namespace backend {
    class VulkanTexture {
    public:
        void LoadFromFile(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool cmdPool, VkQueue queue, const std::string& path);
        void Destroy(VkDevice device);

        VkImageView GetImageView() const { return imageView; }
        VkSampler GetSampler() const { return sampler; }

    private:
        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkSampler sampler = VK_NULL_HANDLE;

        VkDevice deviceRef = VK_NULL_HANDLE;
        VkPhysicalDevice physicalRef = VK_NULL_HANDLE;

        void TransitionImageLayout(VkCommandPool cmdPool, VkQueue queue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
        void CopyBufferToImage(VkCommandPool cmdPool, VkQueue queue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    };
}
