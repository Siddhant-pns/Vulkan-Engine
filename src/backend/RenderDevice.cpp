#include "backend/RenderDevice.h"
#include "plugins/null/NullBackend.hpp"
#include "plugins/vulkan/VulkanBackend.h"     // NEW thin wrapper (see below)

namespace gfx {

static CmdHandle g_currCmd;

static IRenderBackend* makeBackend(const char* name) {
    if (strcmp(name, "null") == 0)    return createNullBackend();
    if (strcmp(name, "vulkan") == 0)  return createVulkanBackend();
    return nullptr;
}

CmdHandle RenderDevice::beginFrame() {
    g_currCmd = s_backend->beginFrame();
    return g_currCmd;
}
void RenderDevice::endFrame(CmdHandle h) { s_backend->endFrame(h); }
gfx::IRenderBackend* RenderDevice::backend() { return s_backend.get(); }

bool RenderDevice::init(void* native, const char* backendName) {
    s_backend.reset(makeBackend(backendName));
    if (!s_backend) return false;
    if (!s_backend->init(native))    return false;
    core::util::Logger::info("RenderDevice: %s backend initialised", backendName);
    return true;
}

CmdHandle RenderDevice::currentCmd() { return g_currCmd; }


void RenderDevice::shutdown() {
    if (!s_backend) return;
    s_backend->shutdown();
    s_backend.reset();
    core::util::Logger::info("RenderDevice: backend shut down");
}

} // namespace gfx
