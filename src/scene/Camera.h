#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace scene {

class Camera {
public:
    Camera() = default;

    /* viewport / projection ------------------------------------------------- */
    void SetViewportSize(float w, float h) { aspect = w / h; }
    void SetFOV(float f)                   { fov = f;        }
    void SetNearFar(float n, float f)      { nearClip = n; farClip = f; }

    /* per-frame ------------------------------------------------------------- */
    void Update(float dt);

    /* view-space transforms ------------------------------------------------- */
    void ProcessMouse(float dx, float dy);     // look
    void ProcessScroll(float dy);              // dolly

    /* WASDQE movement ------------------------------------------------------- */
    void MoveForward (float d);
    void MoveBackward(float d);
    void MoveRight   (float d);
    void MoveLeft    (float d);
    void MoveUp      (float d);
    void MoveDown    (float d);

    /* getters --------------------------------------------------------------- */
    glm::mat4 GetViewMatrix()       const;
    glm::mat4 GetProjectionMatrix() const;
    glm::vec3 GetPosition()         const { return position; }
    float      GetSpeed()           const { return speed;    }
    void       SetSpeed(float s)          { speed = s;       }

private:
    /* state ----------------------------------------------------------------- */
    glm::vec3 position { 0.0f, 0.0f,  3.0f };
    glm::vec3 front    { 0.0f, 0.0f, -1.0f };
    glm::vec3 up       { 0.0f, 1.0f,  0.0f };

    float yaw   =  -90.0f;   // start facing -Z
    float pitch =    0.0f;

    /* projection */
    float fov       = 45.0f;
    float aspect    = 16.0f/9.0f;
    float nearClip  = 0.1f;
    float farClip   = 100.0f;

    /* tuning */
    float speed        =  5.0f;   // world units / second
    float mouseSens    =  0.1f;   // degrees / pixel
    float scrollSens   =  2.0f;   // units  / wheel-tick
};

} // namespace scene
