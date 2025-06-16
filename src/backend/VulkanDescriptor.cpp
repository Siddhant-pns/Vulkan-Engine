#include "VulkanDescriptor.h"
#include "VulkanUtils.h"
#include <iostream>
#include <array>

namespace backend {

void VulkanDescriptor::Create(VkDevice device) {
    // Set 0: Combined image sampler
    VkDescriptorSetLayoutBinding samplerBinding{};
    samplerBinding.binding = 0;
    samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBinding.descriptorCount = 1;
    samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerBinding.pImmutableSamplers = nullptr;

    // Uniform Buffer Object (set = 1, used in vertex shader)
    VkDescriptorSetLayoutBinding uboBinding{};
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT; // ✅ FIXED HERE
    uboBinding.pImmutableSamplers = nullptr;

    std::array<VkDescriptorSetLayoutBinding, 1> samplerBindings = { samplerBinding };
    std::array<VkDescriptorSetLayoutBinding, 1> uboBindings = { uboBinding };

    VkDescriptorSetLayoutCreateInfo samplerLayoutInfo{};
    samplerLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    samplerLayoutInfo.bindingCount = static_cast<uint32_t>(samplerBindings.size());
    samplerLayoutInfo.pBindings = samplerBindings.data();

    VkDescriptorSetLayoutCreateInfo uboLayoutInfo{};
    uboLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    uboLayoutInfo.bindingCount = static_cast<uint32_t>(uboBindings.size());
    uboLayoutInfo.pBindings = uboBindings.data();

    CheckVkResult(vkCreateDescriptorSetLayout(device, &samplerLayoutInfo, nullptr, &layout0),
        "Failed to create descriptor set layout 0");
    CheckVkResult(vkCreateDescriptorSetLayout(device, &uboLayoutInfo, nullptr, &layout1),
        "Failed to create descriptor set layout 1");

    // Create descriptor pool
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = 1;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[1].descriptorCount = 1;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.maxSets = 2;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();

    CheckVkResult(vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool), "Failed to create descriptor pool");
}

void VulkanDescriptor::Allocate(VkDevice device) {
    VkDescriptorSetAllocateInfo allocInfo0{};
    allocInfo0.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo0.descriptorPool = pool;
    allocInfo0.descriptorSetCount = 1;
    allocInfo0.pSetLayouts = &layout0;
    CheckVkResult(vkAllocateDescriptorSets(device, &allocInfo0, &set0), "Failed to allocate descriptor set 0");

    VkDescriptorSetAllocateInfo allocInfo1{};
    allocInfo1.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo1.descriptorPool = pool;
    allocInfo1.descriptorSetCount = 1;
    allocInfo1.pSetLayouts = &layout1;
    CheckVkResult(vkAllocateDescriptorSets(device, &allocInfo1, &set1), "Failed to allocate descriptor set 1");
}

void VulkanDescriptor::Update(VkDevice device, VkImageView imageView, VkSampler sampler) {
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = imageView;
    imageInfo.sampler = sampler;

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set0;
    write.dstBinding = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void VulkanDescriptor::Update(VkDevice device, const VkDescriptorBufferInfo& bufferInfo) {
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = set1;
    write.dstBinding = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

void VulkanDescriptor::Destroy(VkDevice device) {
    if (layout0) vkDestroyDescriptorSetLayout(device, layout0, nullptr);
    if (layout1) vkDestroyDescriptorSetLayout(device, layout1, nullptr);
    if (pool) vkDestroyDescriptorPool(device, pool, nullptr);
}

}
