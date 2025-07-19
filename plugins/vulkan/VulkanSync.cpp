#include "VulkanSync.h"
#include <iostream>

namespace backend {
void VulkanSync::Create(VkDevice device) {
    VkSemaphoreCreateInfo sci{VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
    vkCreateSemaphore(device, &sci, nullptr, &imageAvailable);
    vkCreateSemaphore(device, &sci, nullptr, &renderFinished);

    VkFenceCreateInfo fci{VK_STRUCTURE_TYPE_FENCE_CREATE_INFO};
    fci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(device, &fci, nullptr, &inFlight);
    std::cout << "[VulkanSync] Synchronization objects created.\n";
}

void VulkanSync::Destroy(VkDevice device) {
    if (imageAvailable)
        vkDestroySemaphore(device, imageAvailable, nullptr);
    if (renderFinished)
        vkDestroySemaphore(device, renderFinished, nullptr);
    if (inFlight)
        vkDestroyFence(device, inFlight, nullptr);
}
} // namespace backend
