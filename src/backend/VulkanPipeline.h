#pragma once
#include <volk.h>
#include <vector>

namespace backend {
    class VulkanPipeline {
    public:
        void CreateRenderPass(VkDevice device, VkFormat swapchainFormat);
        void CreateFramebuffers(VkDevice device, VkExtent2D extent, const std::vector<VkImageView>& imageViews);
        void Destroy(VkDevice device);

        VkRenderPass GetRenderPass() const { return renderPass; }
        const std::vector<VkFramebuffer>& GetFramebuffers() const { return framebuffers; }

        void CreateGraphicsPipeline(VkDevice device, VkExtent2D extent, VkRenderPass renderPass,
                            VkShaderModule vertShader, VkShaderModule fragShader);
        VkPipeline GetPipeline() const { return pipeline; }
        VkPipelineLayout GetPipelineLayout() const { return layout; }


    private:
        VkRenderPass renderPass = VK_NULL_HANDLE;
        std::vector<VkFramebuffer> framebuffers;
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout layout = VK_NULL_HANDLE;
    };
}
