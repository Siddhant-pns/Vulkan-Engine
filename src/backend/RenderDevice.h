#pragma once
#include "backend/include/IRenderBackend.h"
#include "core/util/Logger.h"
#include "plugins/vulkan/VulkanBackend.h" // for VulkanBackend
#include <memory>

namespace gfx {

/* Thin façade that owns the chosen backend instance. */
class RenderDevice {
  public:
    static bool init(void* nativeWindowHandle, const char* backendName = "vulkan");
    static void shutdown();
    static void preShutdown() {
        if (s_backend)
            s_backend->preShutdown();
    }

    static gfx::CmdHandle beginFrame();
    static void endFrame(gfx::CmdHandle);

    /* resource helpers forwarded to backend */
    static gfx::TextureHandle createTexture(const gfx::TextureDesc& d) {
        return s_backend->createTexture(d);
    }
    static gfx::BufferHandle createBuffer(const gfx::BufferDesc& d) {
        return s_backend->createBuffer(d);
    }
    static gfx::CmdHandle currentCmd();
    static gfx::IRenderBackend* backend(); // declaration

  private:
    static inline std::unique_ptr<IRenderBackend> s_backend;
};

} // namespace gfx
