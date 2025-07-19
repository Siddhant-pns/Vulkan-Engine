#pragma once
#include <vector>
#include <volk.h>

namespace backend {
class VulkanSwapchain {
  public:
    void Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface, VkExtent2D size,
                VkCommandPool commandPool, VkQueue graphicsQueue);
    void Recreate();
    void Destroy();

    VkSwapchainKHR Get() const {
        return swapchain;
    }
    const VkSwapchainKHR& GetHandle() const {
        return swapchain;
    }

    // Add these under public:
    VkFormat GetFormat() const {
        return imageFormat;
    }
    void SetCommandPool(VkCommandPool pool);
    VkExtent2D GetExtent() const {
        return extent;
    }
    const std::vector<VkImageView>& GetImageViews() const {
        return imageViews;
    }

    VkImageView GetDepthImageView() const {
        return depthImageView;
    }
    VkFormat GetDepthFormat() const {
        return depthFormat;
    }

    VkImage CurrentImage(uint32_t frameIndex) const {
        return images[frameIndex];
    }

  private:
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkPhysicalDevice physicalDeviceRef = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchainHandle = VK_NULL_HANDLE;
    VkDevice deviceRef = VK_NULL_HANDLE;

    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;

    VkFormat imageFormat;
    VkExtent2D extent;

    // Helper functions
    VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes);
    VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    VkImage depthImage = VK_NULL_HANDLE;
    VkDeviceMemory depthImageMemory = VK_NULL_HANDLE;
    VkImageView depthImageView = VK_NULL_HANDLE;
    VkFormat depthFormat = VK_FORMAT_UNDEFINED;

    void CreateDepthResources(VkDevice device, VkPhysicalDevice physical, VkExtent2D extent, VkCommandPool commandPool,
                              VkQueue queue);
    VkFormat FindDepthFormat(VkPhysicalDevice physical);
};
} // namespace backend
