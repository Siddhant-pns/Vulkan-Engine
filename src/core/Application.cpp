#include "Application.h"
#include "platform/Window.h"
#include "backend/VulkanRenderer.h"
#include <iostream>

static platform::Window* window = nullptr;
static backend::VulkanRenderer* gRenderer = nullptr;

void core::Application::Init() {
    std::cout << "[Application] Initializing...\n";
    window = new platform::Window(1280, 720, "Vulkan Engine");
    window->Init();

    gRenderer = new backend::VulkanRenderer();
    gRenderer->Init(window);
}

void core::Application::Run() {
    std::cout << "[Application] Running main loop...\n";
    while (!window->ShouldClose()) {
        window->PollEvents();
        gRenderer->BeginFrame();
        gRenderer->EndFrame();
    }
}

void core::Application::Shutdown() {
    std::cout << "[Application] Shutting down...\n";
    gRenderer->Cleanup();
    delete gRenderer;
    window->Cleanup();
    delete window;
}
