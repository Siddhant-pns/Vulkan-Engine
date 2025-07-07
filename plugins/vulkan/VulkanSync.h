#pragma once
#include <volk.h>

namespace backend {
    class VulkanSync {
    public:
        void Create(VkDevice device);
        void Destroy();

    private:
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;
        VkFence inFlight = VK_NULL_HANDLE;
    };
}
