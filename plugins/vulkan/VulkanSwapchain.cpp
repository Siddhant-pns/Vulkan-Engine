#include "VulkanSwapchain.h"
#include "VulkanUtils.h"
#include <iostream>

namespace backend {

void VulkanSwapchain::Create(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
                            VkExtent2D size, VkCommandPool commandPool, VkQueue graphicsQueue) {
    std::cout << "[VulkanSwapchain] Creating swapchain...\n";
    deviceRef = device;
    physicalDeviceRef = physicalDevice;

    // Query surface capabilities
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceRef, surface, &capabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceRef, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDeviceRef, surface, &formatCount, formats.data());

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceRef, surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDeviceRef, surface, &presentModeCount, presentModes.data());

    // Select format, mode, extent
    VkSurfaceFormatKHR surfaceFormat = ChooseSurfaceFormat(formats);
    VkPresentModeKHR presentMode = ChoosePresentMode(presentModes);
    extent = ChooseExtent(capabilities);
    imageFormat = surfaceFormat.format;

    // Create swapchain
    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                            VK_IMAGE_USAGE_TRANSFER_DST_BIT; // for blitting

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    CheckVkResult(vkCreateSwapchainKHR(deviceRef, &createInfo, nullptr, &swapchain),
                  "Failed to create swapchain");

    // Get swapchain images
    uint32_t actualImageCount = 0;
    vkGetSwapchainImagesKHR(deviceRef, swapchain, &actualImageCount, nullptr);
    images.resize(actualImageCount);
    vkGetSwapchainImagesKHR(deviceRef, swapchain, &actualImageCount, images.data());

    // Create image views
    imageViews.resize(images.size());
    for (size_t i = 0; i < images.size(); ++i) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = imageFormat;
        viewInfo.components = {
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
            VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY
        };
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        CheckVkResult(vkCreateImageView(deviceRef, &viewInfo, nullptr, &imageViews[i]),
                      "Failed to create image view");
    }

    // Create depth resources
    depthFormat = FindDepthFormat(physicalDeviceRef);
    CreateDepthResources(device, physicalDevice, extent, commandPool, graphicsQueue);


    std::cout << "[VulkanSwapchain] Swapchain created with " << imageViews.size() << " image views.\n";
}

VkFormat VulkanSwapchain::FindDepthFormat(VkPhysicalDevice physical) {
    std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT
    };

    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physical, format, &props);
        if (props.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            return format;
        }
    }

    throw std::runtime_error("Failed to find supported depth format!");
}
 
void VulkanSwapchain::CreateDepthResources(VkDevice device, VkPhysicalDevice physical, VkExtent2D extent, VkCommandPool commandPool, VkQueue queue) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CheckVkResult(vkCreateImage(device, &imageInfo, nullptr, &depthImage), "Failed to create depth image");

    VkMemoryRequirements memReqs;
    vkGetImageMemoryRequirements(device, depthImage, &memReqs);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReqs.size;

    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physical, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i) {
        if ((memReqs.memoryTypeBits & (1 << i)) &&
            (memProps.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) {
            allocInfo.memoryTypeIndex = i;
            break;
        }
    }

    CheckVkResult(vkAllocateMemory(device, &allocInfo, nullptr, &depthImageMemory), "Failed to allocate depth memory");
    vkBindImageMemory(device, depthImage, depthImageMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    CheckVkResult(vkCreateImageView(device, &viewInfo, nullptr, &depthImageView), "Failed to create depth image view");
}



void VulkanSwapchain::Destroy() {

    if (depthImageView) vkDestroyImageView(deviceRef, depthImageView, nullptr);
    if (depthImage) vkDestroyImage(deviceRef, depthImage, nullptr);
    if (depthImageMemory) vkFreeMemory(deviceRef, depthImageMemory, nullptr);

    for (auto& view : imageViews) {
        vkDestroyImageView(deviceRef, view, nullptr);
    }
    imageViews.clear();

    if (swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(deviceRef, swapchain, nullptr);
        swapchain = VK_NULL_HANDLE;
    }

    std::cout << "[VulkanSwapchain] Swapchain destroyed.\n";
}

// --- Helper functions ---

VkSurfaceFormatKHR VulkanSwapchain::ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats) {
    for (const auto& f : formats) {
        if (f.format == VK_FORMAT_B8G8R8A8_UNORM && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return f;
    }
    return formats[0]; // fallback
}

VkPresentModeKHR VulkanSwapchain::ChoosePresentMode(const std::vector<VkPresentModeKHR>& modes) {
    for (const auto& m : modes) {
        if (m == VK_PRESENT_MODE_MAILBOX_KHR) return m; // triple buffering
    }
    return VK_PRESENT_MODE_FIFO_KHR; // always supported
}

VkExtent2D VulkanSwapchain::ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
        return {1280, 720}; // fallback if no fixed size
    }
}
// if (capabilities.currentExtent.width != UINT32_MAX) {
//         extent = capabilities.currentExtent; // Already defined by surface
//     } else {
        
//         extent.width  = std::clamp((uint32_t)win->width(), capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
//         extent.height = std::clamp((uint32_t)win->height(), capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
//     }
void VulkanSwapchain::SetCommandPool(VkCommandPool pool) {
    commandPool = pool;
}


}
