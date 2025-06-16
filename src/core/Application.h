#pragma once
#include "scene/Camera.h"

namespace core {
class Application {
public:
    void Init();
    void Run();
    void Shutdown();
    scene::Camera& GetCamera() { return camera; }

private:
    void Tick(float dt);         // per-frame update
    scene::Camera       camera;
    bool dragging   = false;   // already have
    bool firstDrag  = true;    // ✱ add this
    double lastX = 0.0, lastY = 0.0;

};
}
