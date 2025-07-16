#define VULKAN_HPP_TYPESAFE_CONVERSION   // (if you ever use vulkan-hpp)
#include <volk.h>   
#include "Window.h"
#include <stdexcept>

namespace core::platform {

Window::Window(int w, int h, const std::string& title) {
    if (!glfwInit()) throw std::runtime_error("GLFW init failed");
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);     // Vulkan-only
    m_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!m_window) throw std::runtime_error("GLFW window create failed");
}

VkSurfaceKHR Window::CreateVulkanSurface(VkInstance instance) {
    VkSurfaceKHR surf = VK_NULL_HANDLE;
    if (glfwCreateWindowSurface(instance, m_window, nullptr, &surf) != VK_SUCCESS)
        return VK_NULL_HANDLE;
    return surf;
}


Window::~Window() { if (m_window) { glfwDestroyWindow(m_window); glfwTerminate(); } }
bool Window::shouldClose() const { return glfwWindowShouldClose(m_window); }
void Window::pollEvents()       { glfwPollEvents(); }

} // namespace core::platform
