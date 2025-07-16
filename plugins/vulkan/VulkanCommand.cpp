#include "VulkanCommand.h"
#include "VulkanUtils.h"
#include <iostream>

namespace backend {

void VulkanCommand::Create(VkDevice device, uint32_t queueFamilyIndex, uint32_t count) {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndex;

    CheckVkResult(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool),
                  "Failed to create command pool");

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = count;

    commandBuffers.resize(count);
    CheckVkResult(vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()),
                  "Failed to allocate command buffers");

    std::cout << "[VulkanCommand] Allocated " << count << " command buffers.\n";
}

void VulkanCommand::Destroy(VkDevice device) {
    if (!commandPool) return;
    if (!commandBuffers.empty())
        vkFreeCommandBuffers(device, commandPool,
                             static_cast<uint32_t>(commandBuffers.size()),
                             commandBuffers.data());

    vkResetCommandPool(device, commandPool, 0);
    vkDestroyCommandPool(device, commandPool, nullptr);
    commandPool = VK_NULL_HANDLE;
    commandBuffers.clear();

    std::cout << "[VulkanCommand] Command pool and buffers destroyed.\n";
}

VkCommandBuffer VulkanCommand::Get(uint32_t index) const {
    return commandBuffers.at(index);
}

}
