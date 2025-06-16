#include "platform/Input.h"
using namespace platform;

void Input::GetMouseDelta(double& dx, double& dy)
{
    double x, y;  glfwGetCursorPos(s_Window, &x, &y);
    dx = x - s_LastX;  dy = y - s_LastY;
    s_LastX = x;       s_LastY = y;
}
