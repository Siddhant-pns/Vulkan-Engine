#pragma once
#include <string>
#include <volk.h>

namespace backend {
class VulkanShader {
  public:
    bool LoadFromFile(VkDevice device, const std::string& path);
    void Destroy(VkDevice device);

    VkShaderModule Get() const {
        return shaderModule;
    }

  private:
    VkShaderModule shaderModule = VK_NULL_HANDLE;
};
} // namespace backend
