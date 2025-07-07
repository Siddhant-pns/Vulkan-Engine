#pragma once
#include <volk.h>

namespace backend {
    class VulkanDescriptor {
    public:
        void Create(VkDevice device);
        void Allocate(VkDevice device);
        void Update(VkDevice device, VkImageView imageView, VkSampler sampler);
        void Update(VkDevice device, const VkDescriptorBufferInfo& bufferInfo);  
        
        void Destroy(VkDevice device);

        VkDescriptorSetLayout GetLayout0() const { return layout0; }
        VkDescriptorSetLayout GetLayout1() const { return layout1; }
        VkDescriptorSet GetSet0() const { return set0; }
        VkDescriptorSet GetSet1() const { return set1; }


    private:
        VkDescriptorSetLayout layout0 = VK_NULL_HANDLE;
        VkDescriptorSetLayout layout1 = VK_NULL_HANDLE;
        VkDescriptorPool pool = VK_NULL_HANDLE;
        VkDescriptorSet set0 = VK_NULL_HANDLE;
        VkDescriptorSet set1 = VK_NULL_HANDLE;

    };
}
