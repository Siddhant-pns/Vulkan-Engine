#include "VulkanUtils.h"
#include <iostream>

namespace backend {
    void CheckVkResult(VkResult result, const std::string& message) {
        if (result != VK_SUCCESS) {
            std::cerr << "[Vulkan Error] " << message << " (code: " << result << ")\n";
            std::exit(EXIT_FAILURE);
        }
    }
}
