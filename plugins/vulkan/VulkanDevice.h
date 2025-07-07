#pragma once
#include <volk.h>

namespace backend {
    class VulkanDevice {
    public:
        void Create(VkInstance instance, VkSurfaceKHR surface);
        void Destroy();

        VkDevice GetLogical() const { return device; }
        VkPhysicalDevice GetPhysical() const { return physicalDevice; }
        VkQueue GetGraphicsQueue() const { return graphicsQueue; }

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;

    };
}
