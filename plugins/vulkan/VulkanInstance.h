#pragma once
#define VK_NO_PROTOTYPES 
#include <volk.h>
#include <vector>
#include <string>

namespace backend {
    class VulkanInstance {
    public:
        void Create(bool enableValidation);
        void Destroy();

        VkInstance Get() const { return instance; }

    private:
        VkInstance instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

        void SetupDebugMessenger();
        bool CheckValidationSupport();
        std::vector<const char*> GetRequiredExtensions();
    };
}
