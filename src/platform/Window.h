#pragma once
#include <GLFW/glfw3.h>
#include <volk.h>
#include <string>

namespace platform {
    class Window {
    public:
        Window(int width, int height, const std::string& title);
        void Init();
        void PollEvents();
        bool ShouldClose();
        void Cleanup();

        GLFWwindow* GetGLFWwindow() const { return window; }
        VkSurfaceKHR CreateVulkanSurface(VkInstance instance);

    private:
        int width, height;
        std::string title;
        GLFWwindow* window = nullptr;
    };
}
