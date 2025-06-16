#pragma once
#include <GLFW/glfw3.h>
#include <volk.h>
#include <string>
#include <glm/glm.hpp>

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
        glm::uvec2 GetFramebufferSize() const;

        bool resized = false; 
        bool WasResized() {
        bool was = resized;
        resized = false;
        return was;
    }
        float GetScrollAndReset() { float s = g_Scroll; g_Scroll = 0.0f; return s; }


    private:
    static float g_Scroll;
    static void ScrollCB(GLFWwindow*,double, double y){ g_Scroll = (float)y; }
        int width, height;
        std::string title;
        GLFWwindow* window = nullptr;
    };
}
