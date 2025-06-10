#include "core/Application.h"

int main() {
    core::Application app;
    app.Init();
    app.Run();
    app.Shutdown();
    return 0;
}
