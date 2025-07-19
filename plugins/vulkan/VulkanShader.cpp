#include "VulkanShader.h"
#include "VulkanUtils.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace backend {

bool VulkanShader::LoadFromFile(VkDevice device, const std::string& path) {
    std::ifstream file(path, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "[VulkanShader] Failed to open shader file: " << path << "\n";
        return false;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = buffer.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    CheckVkResult(result, "Failed to create shader module");

    std::cout << "[VulkanShader] Shader loaded: " << path << "\n";
    return true;
}

void VulkanShader::Destroy(VkDevice device) {
    if (shaderModule != VK_NULL_HANDLE) {
        vkDestroyShaderModule(device, shaderModule, nullptr);
        shaderModule = VK_NULL_HANDLE;
    }
}

} // namespace backend
