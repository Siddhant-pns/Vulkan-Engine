#include "core/Application.h"
#include <renderer/RenderAPI.h>

int main() {
    core::Application app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}
