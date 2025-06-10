#pragma once

namespace core {
    class Application {
    public:
        void Init();      // Initialize window, renderer, resources
        void Run();       // Main loop
        void Shutdown();  // Cleanup
    private:
        void ProcessInput();
        void Update();
        void Render();
    };
}

