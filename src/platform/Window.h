#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>

typedef struct VkInstance_T*   VkInstance;
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;

namespace core::platform {

class Window {
public:
    Window(int w, int h, const std::string& title);
    ~Window();

    bool shouldClose() const;
    void pollEvents();
    void* nativeHandle() const { return m_window; }   // GLFWwindow*

    VkSurfaceKHR CreateVulkanSurface(VkInstance instance);

private:
    GLFWwindow* m_window{};
};

} // namespace core::platform
