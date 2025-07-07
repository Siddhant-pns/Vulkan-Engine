#include "VulkanBuffer.h"
#include "VulkanUtils.h"
#include <iostream>

namespace backend {

void VulkanBuffer::Create(VkDevice device, VkPhysicalDevice physicalDevice,
                          VkDeviceSize size, VkBufferUsageFlags usage,
                          VkMemoryPropertyFlags properties) {
    deviceRef = device;
    physicalDeviceRef = physicalDevice;
    bufferSize = size;

    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = size;
    info.usage = usage;
    info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CheckVkResult(vkCreateBuffer(device, &info, nullptr, &buffer), "Failed to create buffer");

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(device, buffer, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(memReq.memoryTypeBits, properties);

    CheckVkResult(vkAllocateMemory(device, &allocInfo, nullptr, &memory), "Failed to allocate buffer memory");

    vkBindBufferMemory(device, buffer, memory, 0);
}

void VulkanBuffer::Upload(const void* data, VkDeviceSize size) {
    void* mapped;
    vkMapMemory(deviceRef, memory, 0, size, 0, &mapped);
    memcpy(mapped, data, static_cast<size_t>(size));
    vkUnmapMemory(deviceRef, memory);
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
        if ((typeFilter & (1 << i)) &&
            (memProps.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    std::cerr << "[VulkanBuffer] Failed to find suitable memory type.\n";
    std::exit(EXIT_FAILURE);
}

}