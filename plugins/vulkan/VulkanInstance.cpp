#define VK_NO_PROTOTYPES
#include <volk.h>

#include "VulkanInstance.h"
#include "VulkanUtils.h"
#include "core/util/Logger.h"
#include <GLFW/glfw3.h>
#include <cstring>
#include <iostream>
#include <vector>

const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

// Implementations will be filled in later
namespace backend {
void backend::VulkanInstance::Create(bool enableValidation) {

    std::cout << "[VulkanInstance] Creating Vulkan instance...\n";

    // ✅ Initialize Volk loader BEFORE any Vulkan function is called
    if (volkInitialize() != VK_SUCCESS) {
        std::cerr << "[VulkanInstance] volkInitialize() failed!\n";
        std::exit(EXIT_FAILURE);
    }

    if (enableValidation && !CheckValidationSupport()) {
        std::cerr << "[VulkanInstance] Validation layers requested but not available!\n";
        std::exit(EXIT_FAILURE);
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "CustomEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    auto extensions = GetRequiredExtensions();

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // std::cout << "[VulkanInstance] Enabled extensions count: " << createInfo.enabledExtensionCount << "\n";
    if (enableValidation) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    if (result != VK_SUCCESS) {
        std::cerr << "[VulkanInstance] vkCreateInstance FAILED! Code: " << result << "\n";
        std::exit(EXIT_FAILURE);
    } else {
        std::cout << "[VulkanInstance] vkCreateInstance succeeded.\n";
    }
    backend::CheckVkResult(result, "Failed to create Vulkan instance");

    volkLoadInstance(instance);

    if (enableValidation)
        SetupDebugMessenger();

    std::cout << "[VulkanInstance] Vulkan instance created.\n";
}

void backend::VulkanInstance::Destroy() {
    if (debugMessenger) {
        auto destroyDebugUtilsMessengerEXT =
            (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");

        if (destroyDebugUtilsMessengerEXT) {
            destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
    }

    if (instance) {
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    std::cout << "[VulkanInstance] Destroyed Vulkan instance.\n";
}

void backend::VulkanInstance::SetupDebugMessenger() {
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

    createInfo.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*) -> VkBool32 {
        std::cerr << "[Validation] " << pCallbackData->pMessage << "\n";
        return VK_FALSE;
    };

    auto createDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (createDebugUtilsMessengerEXT &&
        createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) == VK_SUCCESS) {
        std::cout << "[VulkanInstance] Debug messenger created.\n";
    } else {
        std::cerr << "[VulkanInstance] Failed to set up debug messenger.\n";
    }
}

bool backend::VulkanInstance::CheckValidationSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool found = false;
        for (const auto& layer : availableLayers) {
            if (strcmp(layerName, layer.layerName) == 0) {
                found = true;
                break;
            }
        }
        if (!found)
            return false;
    }

    return true;
}

std::vector<const char*> backend::VulkanInstance::GetRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME); // for validation

    return extensions;
}

} // namespace backend
