#pragma once
#include <volk.h>
#include "VulkanDevice.h"

namespace backend {

    class VulkanSync {
    public:
        void Create(VkDevice device);
        void Destroy(VkDevice device);
        VkSemaphore getImageAvailable() const { return imageAvailable; }
        VkSemaphore getRenderFinished() const { return renderFinished; }
        VkFence getInFlight() const { return inFlight; }

        VkFence inFlight = VK_NULL_HANDLE;

    private:
        backend::VulkanDevice   m_device;
        VkSemaphore imageAvailable = VK_NULL_HANDLE;
        VkSemaphore renderFinished = VK_NULL_HANDLE;
    };
}
