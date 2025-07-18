#pragma once
#include <volk.h>
#include <glm/glm.hpp>
#include <vector>

namespace backend {

    class VulkanUniformBuffer {
    public:
        void Create(VkDevice device, VkPhysicalDevice physical, size_t bufferSize, size_t frameCount);
        void Destroy(VkDevice device);

        void Update(VkDevice device, uint32_t currentFrame, const void* data, size_t size);

        VkBuffer GetBuffer(uint32_t frame) const { return buffers[frame]; }
        VkDescriptorBufferInfo GetDescriptorInfo(uint32_t frame) const;

        uint32_t InstanceCount() const { return m_count; } 

        inline size_t alignTo(size_t value, size_t alignment) {
            return (value + alignment - 1) & ~(alignment - 1);
        }

    private:
        std::vector<VkBuffer> buffers;
        std::vector<VkDeviceMemory> memories;
        void*           m_mapped   {nullptr};
        size_t          m_stride   {0};
        size_t          m_size     {0};
        size_t frameCount = 0;
        uint32_t m_count = 0;
    };

}
