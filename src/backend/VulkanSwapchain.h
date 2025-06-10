#pragma once
#include <volk.h>
#include <vector>

namespace backend {
    class VulkanSwapchain {
    public:
        void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface);
        void Recreate();
        void Destroy();

        VkSwapchainKHR Get() const { return swapchain; }
        // Add these under public:
        VkFormat GetFormat() const { return imageFormat; }
        VkExtent2D GetExtent() const { return extent; }
        const std::vector<VkImageView>& GetImageViews() const { return imageViews; }

    private:
        VkPhysicalDevice physicalDeviceRef = VK_NULL_HANDLE;
        VkSwapchainKHR swapchain = VK_NULL_HANDLE;
        VkDevice deviceRef = VK_NULL_HANDLE;

        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;

        VkFormat imageFormat;
        VkExtent2D extent;

        // Helper functions
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes);
        VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    };
}
