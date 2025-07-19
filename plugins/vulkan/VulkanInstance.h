#pragma once
#define VK_NO_PROTOTYPES
#include <string>
#include <vector>
#include <volk.h>

namespace backend {
class VulkanInstance {
  public:
    void Create(bool enableValidation);
    void Destroy();

    VkInstance Get() const {
        return instance;
    }

  private:
    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

    void SetupDebugMessenger();
    bool CheckValidationSupport();
    std::vector<const char*> GetRequiredExtensions();
};
} // namespace backend
