#include "Camera.h"

namespace scene {

void Camera::Update(float)
{
    pitch = glm::clamp(pitch, -89.0f, 89.0f);

    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    front   = glm::normalize(front);
}

void Camera::ProcessMouse(float dx, float dy)
{
    yaw   += dx * mouseSens;
    pitch += dy * mouseSens;           // invert Y
}

void Camera::ProcessScroll(float dy)
{
    position += front * (dy * scrollSens);
}

void Camera::MoveForward (float d) { position += front * d; }
void Camera::MoveBackward(float d) { position -= front * d; }
void Camera::MoveRight   (float d) { position += glm::normalize(glm::cross(front, up)) * d; }
void Camera::MoveLeft    (float d) { position -= glm::normalize(glm::cross(front, up)) * d; }
void Camera::MoveUp      (float d) { position += up * d; }
void Camera::MoveDown    (float d) { position -= up * d; }

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    glm::mat4 proj = glm::perspective(glm::radians(fov), aspect, nearClip, farClip);
    proj[1][1] *= -1.0f;                       // flip Y for Vulkan
    return proj;
}

} // namespace scene
