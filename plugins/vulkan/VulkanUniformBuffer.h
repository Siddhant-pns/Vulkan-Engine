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

    private:
        std::vector<VkBuffer> buffers;
        std::vector<VkDeviceMemory> memories;
        size_t frameCount = 0;
    };

}
