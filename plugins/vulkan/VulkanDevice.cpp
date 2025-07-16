#include "VulkanDevice.h"
#include "VulkanUtils.h"
#include <iostream>
#include <vector>
#include <set>
#include <cstring> // for strcmp

const std::vector<const char*> requiredExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

namespace backend {

void VulkanDevice::Create(VkInstance instance, VkSurfaceKHR surface) {
    std::cout << "[VulkanDevice] Enumerating physical devices...\n";

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        std::cerr << "[VulkanDevice] No Vulkan-compatible GPUs found!\n";
        std::exit(EXIT_FAILURE);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    std::cout << "[VulkanDevice] Found " << deviceCount << " GPU(s)\n";

    // Pick first discrete GPU (fallback: first available)
    for (const auto& dev : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(dev, &props);

        if (props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            m_physicalDevice = dev;
            std::cout << "[VulkanDevice] Selected discrete GPU: " << props.deviceName << "\n";
            break;
        }
    }

    if (m_physicalDevice == VK_NULL_HANDLE) {
        m_physicalDevice = devices[0];
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(m_physicalDevice, &props);
        std::cout << "[VulkanDevice] Fallback GPU selected: " << props.deviceName << "\n";
    }

    // Find queue family index with graphics support
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, queueFamilies.data());

    uint32_t graphicsQueueFamily = -1;
    for (uint32_t i = 0; i < queueFamilyCount; ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamily = i;
            break;
        }
    }

    if (graphicsQueueFamily == -1) {
        std::cerr << "[VulkanDevice] Failed to find graphics queue family!\n";
        std::exit(EXIT_FAILURE);
    }

    // Device Queue
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = graphicsQueueFamily;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

    // Device Features
    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueInfo;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();


    VkResult result = vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device);
    m_graphicsFamily = graphicsQueueFamily;
    volkLoadDevice(m_device);           
    vkGetDeviceQueue(m_device, graphicsQueueFamily, 0, &m_graphicsQueue);
    backend::CheckVkResult(result, "Failed to create logical m_device");


    std::cout << "[VulkanDevice] Logical m_device created.\n";
}

void VulkanDevice::Destroy() {
    if (m_device) {
        vkDestroyDevice(m_device, nullptr);
        m_device = VK_NULL_HANDLE;
    }
}

}
