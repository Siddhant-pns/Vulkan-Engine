#include "VulkanUniformBuffer.h"
#include "VulkanUtils.h"
#include <stdexcept>
#include <cstring>

namespace backend {

void VulkanUniformBuffer::Create(VkDevice device, VkPhysicalDevice physical, size_t bufferSize, size_t inFrameCount) {
    frameCount = inFrameCount;
    m_count = inFrameCount;  
    m_stride = alignTo(bufferSize, 256);                // std140 align
    m_size   = m_stride * m_count;
    buffers.resize(frameCount);
    memories.resize(frameCount);

    for (size_t i = 0; i < frameCount; ++i) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        CheckVkResult(vkCreateBuffer(device, &bufferInfo, nullptr, &buffers[i]), "Failed to create uniform buffer");

        VkMemoryRequirements memReq;
        vkGetBufferMemoryRequirements(device, buffers[i], &memReq);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memReq.size;

        VkPhysicalDeviceMemoryProperties memProps;
        vkGetPhysicalDeviceMemoryProperties(physical, &memProps);

        bool found = false;
        const VkMemoryPropertyFlags wanted =
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

        for (uint32_t j = 0; j < memProps.memoryTypeCount; ++j) {
            if ( (memReq.memoryTypeBits & (1u << j)) &&
                ( (memProps.memoryTypes[j].propertyFlags & wanted) == wanted ) ) {
                allocInfo.memoryTypeIndex = j;
                found = true;
                break;
            }
        }

        if (!found) throw std::runtime_error("Failed to find suitable memory type for uniform buffer");

        CheckVkResult(vkAllocateMemory(device, &allocInfo, nullptr, &memories[i]), "Failed to allocate uniform buffer memory");
        CheckVkResult(vkBindBufferMemory(device, buffers[i], memories[i], 0), "Failed to bind uniform buffer memory");
    }
}

void VulkanUniformBuffer::Destroy(VkDevice device) {
    for (size_t i = 0; i < frameCount; ++i) {
        if (buffers[i]) vkDestroyBuffer(device, buffers[i], nullptr);
        if (memories[i]) vkFreeMemory(device, memories[i], nullptr);
    }
    buffers.clear();
    memories.clear();
    frameCount = 0;
}

void VulkanUniformBuffer::Update(VkDevice device,uint32_t currentFrame, const void* data, size_t size) {
    void* mapped;
    vkMapMemory(device, memories[currentFrame], 0, size, 0, &mapped);
    std::memcpy(mapped, data, size);
    vkUnmapMemory(device, memories[currentFrame]);
}

VkDescriptorBufferInfo VulkanUniformBuffer::GetDescriptorInfo(uint32_t frame) const {
    VkDescriptorBufferInfo info{};
    info.buffer = buffers[frame];
    info.offset = 0;
    info.range = VK_WHOLE_SIZE;
    return info;
}

}
