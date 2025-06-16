#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h> 
#include <GLFW/glfw3.h>

#include "Window.h"
#include <iostream>
#include <glm/glm.hpp> // Include glm for uvec2


float platform::Window::g_Scroll = 0.0f;

platform::Window::Window(int width, int height, const std::string& title)
    : width(width), height(height), title(title) {}

void platform::Window::Init() {
    if (!glfwInit()) {
        std::cerr << "[Window] GLFW initialization failed!\n";
        exit(-1);
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // no OpenGL
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "[Window] Window creation failed!\n";
        glfwTerminate();
        exit(-1);
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* w, int, int) {
    auto win = static_cast<Window*>(glfwGetWindowUserPointer(w));
    win->resized = true;
});

    glfwSetScrollCallback(window, ScrollCB);

    std::cout << "[Window] Created " << width << "x" << height << " window.\n";
}

VkSurfaceKHR platform::Window::CreateVulkanSurface(VkInstance instance) {
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        std::cerr << "[Window] Failed to create Vulkan surface.\n";
        return VK_NULL_HANDLE;
    }
    return surface;
}

glm::uvec2 platform::Window::GetFramebufferSize() const {
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    return glm::uvec2(width, height);
}


void platform::Window::PollEvents() {
    glfwPollEvents();
}

bool platform::Window::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void platform::Window::Cleanup() {
    glfwDestroyWindow(window);
    glfwTerminate();
}