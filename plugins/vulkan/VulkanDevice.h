#pragma once
#include <GLFW/glfw3.h>
#include <volk.h>

namespace backend {
class VulkanDevice {
  public:
    void Create(VkInstance instance, VkSurfaceKHR surface);
    void Destroy();

    VkPhysicalDevice physical() const noexcept {
        return m_physicalDevice;
    }
    VkDevice logical() const noexcept {
        return m_device;
    }
    VkInstance instance() const noexcept {
        return m_instance;
    }
    VkQueue GetGraphicsQueue() const {
        return m_graphicsQueue;
    }

    uint32_t graphicsFamily() const noexcept {
        return m_graphicsFamily;
    }
    // VkQueue  graphicsQueue()    const noexcept { return m_graphicsQueue; }

  private:
    VkInstance m_instance;
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice{};
    VkDevice m_device{};
    // VkQueue graphicsQueue = VK_NULL_HANDLE;
    uint32_t m_graphicsFamily = 0;
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
};
} // namespace backend
