#include "core/util/core.hpp"
#include "backend/RenderDevice.h"
#include "graphics/render/RenderGraph.h"
#include "platform/Window.h"
#include <iostream>

int main() {
    using core::util::Logger;
    core::platform::Window win{1280, 720, "Sandbox"};
    Logger::init("Sandbox3D");

    if (!gfx::RenderDevice::init(&win, "vulkan")) {
        Logger::error("RenderDevice init failed");   //  see console
        while (!win.shouldClose()) {     // keep window so you can read log
            win.pollEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        return -1;
    }

    /* ----------------- Build graph once ---------------------- */
    gfx::RenderGraph graph;
    graph.addTexture("color", {1280, 720});
    graph.addPass("clear", gfx::PassType::Graphics,
              {}, std::array<std::string, 1>{"color"},
              [](const gfx::RenderPass::ExecCtx& ctx)
              {
                static const float mag[4]{1.f,0.f,1.f,1.f};
                  auto& res = ctx.graph->resource("color");
                  ctx.device->clearColor(ctx.cmd,
                                         std::get<gfx::TextureHandle>(res.handle),
                                         mag);
              });

    graph.compile();

    /* ----------------- Frame loop (one frame for now) -------- */
    uint64_t frame = 0;

    while (!win.shouldClose()) {
        std::cout << "Frame: " << frame << std::endl;
        win.pollEvents();

        auto cmd = gfx::RenderDevice::beginFrame();   // returns CmdHandle
        graph.execute(frame++, cmd);                  // pass both args
        gfx::RenderDevice::endFrame(cmd);
    }

    gfx::RenderDevice::shutdown();
}
