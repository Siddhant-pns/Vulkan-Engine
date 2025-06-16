#pragma once
#include <volk.h>
#include <string>

namespace backend {
    class VulkanShader {
    public:
        bool LoadFromFile(VkDevice device, const std::string& path);
        void Destroy(VkDevice device);

        VkShaderModule Get() const { return shaderModule; }

    private:
        VkShaderModule shaderModule = VK_NULL_HANDLE;
    };
}
