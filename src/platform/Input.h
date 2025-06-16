#pragma once
#include <GLFW/glfw3.h>

namespace platform {
class Input {
public:
    static void Init(GLFWwindow* win) { s_Window = win; }

    static bool  IsKeyPressed     (int key)    { return glfwGetKey (s_Window, key)    == GLFW_PRESS; }
    static bool  IsMousePressed   (int button) { return glfwGetMouseButton(s_Window, button) == GLFW_PRESS; }
    static void  GetMouseDelta    (double& dx, double& dy);
    static void  GetMousePosition(double& x, double& y)        { glfwGetCursorPos(s_Window, &x, &y); }
private:
    static inline GLFWwindow* s_Window = nullptr;
    static inline double s_LastX = 0.0, s_LastY = 0.0;
};
}