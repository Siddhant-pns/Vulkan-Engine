#include "Application.h"
#include "platform/Window.h"
#include "backend/VulkanRenderer.h"
#include "platform/Input.h"
#include <iostream>
#include <chrono>
#include <thread>

static platform::Window* window = nullptr;
static backend::VulkanRenderer* gRenderer = nullptr;

double lastMouseX = 0.0, lastMouseY = 0.0;
bool dragging = false;
float dt = 0.016f;

void core::Application::Init() {
    window = new platform::Window(1280,720,"Vulkan Engine");
    window->Init();
    platform::Input::Init(window->GetGLFWwindow());

    gRenderer = new backend::VulkanRenderer();
    gRenderer->Init(window);
    gRenderer->SetApplication(this);
}

void core::Application::Run() {
    std::cout << "[Application] Running main loop...\n";
    const float targetFrameTime = 1.0f / 60.0f; // 60 FPS
    float last = glfwGetTime();  // At start of loop
    while (!window->ShouldClose()) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        
        float now = (float)glfwGetTime();
        float dt  = now - last;  last = now;

        window->PollEvents();
        Tick(dt);
        gRenderer->BeginFrame();
        gRenderer->EndFrame();

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = frameEnd - frameStart;
        float sleepTime = targetFrameTime - elapsed.count();
        if (sleepTime > 0.0f) {
            std::this_thread::sleep_for(std::chrono::duration<float>(sleepTime));
        }
    }
}

void core::Application::Shutdown() {
    std::cout << "[Application] Shutting down...\n";
    gRenderer->Cleanup();
    delete gRenderer;
    window->Cleanup();
    delete window;
}

void core::Application::Tick(float dt)
{
    /* -- handle mouse -- */
    if (platform::Input::IsMousePressed(GLFW_MOUSE_BUTTON_RIGHT))
{
    double x, y;
    platform::Input::GetMouseDelta(x, y);      // still returns absolute-delta

    if (firstDrag)            // first frame after RMB pressed
    {
        /*  reset stored cursor origin so we don’t get a huge jump   */
        platform::Input::GetMousePosition(lastX, lastY);
        firstDrag = false;
    }
    else
    {
        /*  compute delta from last frame manually                    */
        double currX, currY;
        platform::Input::GetMousePosition(currX, currY);
        double dx = currX - lastX;
        double dy = currY - lastY;
        lastX = currX;
        lastY = currY;

        camera.ProcessMouse((float)dx, (float)dy);
    }

        dragging = true;
    }
    else
    {
        dragging   = false;
        firstDrag  = true;             // next press starts fresh, rotation is kept
    }


    /* -- keyboard WASD QE -- */
    float v = camera.GetSpeed()*dt;
    if (platform::Input::IsKeyPressed(GLFW_KEY_W)) camera.MoveForward (v);
    if (platform::Input::IsKeyPressed(GLFW_KEY_S)) camera.MoveBackward(v);
    if (platform::Input::IsKeyPressed(GLFW_KEY_A)) camera.MoveLeft    (v);
    if (platform::Input::IsKeyPressed(GLFW_KEY_D)) camera.MoveRight   (v);
    if (platform::Input::IsKeyPressed(GLFW_KEY_E)) camera.MoveUp      (v);
    if (platform::Input::IsKeyPressed(GLFW_KEY_Q)) camera.MoveDown    (v);

    /* -- scroll zoom -- */
    camera.ProcessScroll(window->GetScrollAndReset());

    /* -- finalize orientation -- */
    camera.Update(dt);
}
