#include "VulkanBuffer.h"
#include "VulkanUtils.h"
#include <iostream>

namespace backend {

void VulkanBuffer::Create(VkDevice device, VkPhysicalDevice physical, VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties) {
    deviceRef = device;
    physicalDeviceRef = physical;
    bufferSize = size;
    memFlags = properties; // ← store flags

    VkBufferCreateInfo bci{VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
    bci.size = size;
    bci.usage = usage;
    bci.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CheckVkResult(vkCreateBuffer(device, &bci, nullptr, &buffer), "Failed to create buffer");

    VkMemoryRequirements req;
    vkGetBufferMemoryRequirements(device, buffer, &req);

    VkMemoryAllocateInfo ai{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
    ai.allocationSize = req.size;
    ai.memoryTypeIndex = FindMemoryType(req.memoryTypeBits, properties);

    CheckVkResult(vkAllocateMemory(device, &ai, nullptr, &memory), "Failed to allocate buffer memory");

    vkBindBufferMemory(device, buffer, memory, 0);
}

/* ---------- Upload ------------------------------------------------------- */
static VkCommandBuffer beginOneShot(VkDevice dev, VkCommandPool pool) {
    VkCommandBufferAllocateInfo ai{VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO};
    ai.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    ai.commandBufferCount = 1;
    ai.commandPool = pool;

    VkCommandBuffer cmd;
    vkAllocateCommandBuffers(dev, &ai, &cmd);

    VkCommandBufferBeginInfo bi{VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO};
    bi.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd, &bi);
    return cmd;
}
static void endOneShot(VkDevice dev, VkCommandPool pool, VkQueue q, VkCommandBuffer cmd) {
    vkEndCommandBuffer(cmd);
    VkSubmitInfo si{VK_STRUCTURE_TYPE_SUBMIT_INFO};
    si.commandBufferCount = 1;
    si.pCommandBuffers = &cmd;
    vkQueueSubmit(q, 1, &si, VK_NULL_HANDLE);
    vkQueueWaitIdle(q);
    vkFreeCommandBuffers(dev, pool, 1, &cmd);
}

void VulkanBuffer::Upload(const void* data, VkDeviceSize size, VkCommandPool pool, VkQueue graphicsQ) {
    /* Fast path: buffer itself is HOST_VISIBLE --------------------------- */
    if (memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
        void* mapped;
        vkMapMemory(deviceRef, memory, 0, size, 0, &mapped);
        memcpy(mapped, data, static_cast<size_t>(size));
        if (!(memFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            VkMappedMemoryRange rng{VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE};
            rng.memory = memory;
            rng.offset = 0;
            rng.size = size;
            vkFlushMappedMemoryRanges(deviceRef, 1, &rng);
        }
        vkUnmapMemory(deviceRef, memory);
        return;
    }

    /* Staging path ------------------------------------------------------- */
    VulkanBuffer staging;
    staging.Create(deviceRef, physicalDeviceRef, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    staging.Upload(data, size, pool, graphicsQ); // recursion hits fast path

    VkCommandBuffer cmd = beginOneShot(deviceRef, pool);
    VkBufferCopy copy{0, 0, size};
    vkCmdCopyBuffer(cmd, staging.Get(), buffer, 1, &copy);
    endOneShot(deviceRef, pool, graphicsQ, cmd);

    staging.Destroy(deviceRef);
}

void VulkanBuffer::Destroy(VkDevice device) {
    if (buffer) {
        vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    if (memory) {
        vkFreeMemory(device, memory, nullptr);
        memory = VK_NULL_HANDLE;
    }
}

uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDeviceRef, &memProps);

    for (uint32_t i = 0; i < memProps.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    std::cerr << "[VulkanBuffer] Failed to find suitable memory type.\n";
    std::exit(EXIT_FAILURE);
}

} // namespace backend