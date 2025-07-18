#include "core/util/core.hpp"
#include "backend/RenderDevice.h"
#include "graphics/render/RenderGraph.h"
#include "platform/Window.h"
#include <iostream>
#include "CubeSetup.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
                std::array<std::string, 1>{}, std::array<std::string, 1>{"color"},
                [](const gfx::RenderPass::ExecCtx& ctx)
    {
        static const float mag[4]{1.f,0.f,0.f,1.f};
        auto& res = ctx.graph->resource("color");
        ctx.device->clearColor(ctx.cmd,
            std::get<gfx::TextureHandle>(res.handle), mag);
    });

    graph.addPass("cube", gfx::PassType::Graphics,
              std::array<std::string, 1>{"color"}, std::array<std::string, 1>{"color"},
              [](const gfx::RenderPass::ExecCtx& ctx)
              {
                auto cmd = ctx.cmd;
                auto& P  = cube.pipe;

                ctx.device->cmdBeginRenderPass(cmd, &P,
                    ctx.frame % P.GetFramebuffers().size());

                ctx.device->cmdBindPipeline       (cmd, P.GetPipeline());
                ctx.device->cmdBindDescriptorSets (cmd, P.GetPipelineLayout(),
                                                cube.desc.GetSet0(), cube.desc.GetSet1());
                ctx.device->cmdBindVertexBuffer   (cmd, cube.vb.Get());
                ctx.device->cmdBindIndexBuffer    (cmd, cube.ib.Get(), VK_INDEX_TYPE_UINT16);
                ctx.device->cmdPushConstants      (cmd, P.GetPipelineLayout(), cube.mvp);
                ctx.device->cmdDrawIndexed        (cmd, 36, 1, 0, 0, 0);

                ctx.device->cmdEndRenderPass(cmd);
              });
    
    auto* vkBackend =
    static_cast<gfx::VulkanBackend*>(gfx::RenderDevice::backend());

    InitCube(vkBackend->device(),
            vkBackend->commands().GetPool(),
            vkBackend->graphicsQ(),
            vkBackend->swapchain());

    graph.compile();

    /* ----------------- Frame loop (one frame for now) -------- */
    uint64_t frame = 0;
    static float angle = 0.f;

    while (!win.shouldClose()) {
    // while (frame < 3) {  // just 4 frames for demo
        std::cout << "Frame: " << frame << std::endl;
        win.pollEvents();

        auto cmd = gfx::RenderDevice::beginFrame();   // returns gfx::CmdHandle

        // core::util::Time::tick(); 
        // angle += glm::radians(45.f) * core::util::Time::delta();
        angle += glm::radians(0.1f);   // 5 ° per frame – very visible in 3 frames

        glm::mat4 model = glm::rotate(glm::mat4(1.f), angle, glm::vec3(0,1,0));
        glm::mat4 view  = glm::lookAt(glm::vec3(0,0,5), glm::vec3(0), glm::vec3(0,1,0));
        glm::mat4 proj  = glm::perspective(glm::radians(60.f),
                            (float)win.width() / (float)win.height(), 0.1f, 100.f);
        proj[1][1] *= -1.f;
        cube.mvp = proj * view * model;

        /* write to per-frame UBO slot */
        uint32_t frameIdx = frame % cube.ubo.InstanceCount();
        cube.ubo.Update(vkBackend->device().logical(), frameIdx, &cube.mvp, sizeof(cube.mvp));
        cube.desc.Update(vkBackend->device().logical(), cube.ubo.GetDescriptorInfo(frameIdx));

        graph.execute(frame++, cmd);                  // pass both args
        gfx::RenderDevice::endFrame(cmd);
    }
    gfx::RenderDevice::preShutdown();  // optional, before shutdown()

    DestroyCube(vkBackend->device());

    gfx::RenderDevice::shutdown();
}
