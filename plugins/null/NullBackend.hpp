#pragma once
#include "backend/include/IRenderBackend.h"
#include "core/util/Logger.h"
#include <glm/glm.hpp>  

namespace gfx {

class NullBackend final : public IRenderBackend {
public:
    bool init(void*) override        { core::util::Logger::info("[Null] init"); return true; }
    gfx::CmdHandle beginFrame() override       { core::util::Logger::info("[Null] beginFrame");  return{}; }
    void endFrame(gfx::CmdHandle) override         { core::util::Logger::info("[Null] endFrame");}
    void clearColor(gfx::CmdHandle, TextureHandle, const float[4]) override {}
    void shutdown() override         { core::util::Logger::info("[Null] shutdown"); }
    void preShutdown() override      { core::util::Logger::info("[Null] preShutdown"); }

    TextureHandle createTexture(const TextureDesc&) override { return {}; }
    BufferHandle  createBuffer (const BufferDesc&) override { return {}; }
    void destroyTexture(TextureHandle) override {}
    void destroyBuffer (BufferHandle) override {}

    void cmdBeginRenderPass(CmdHandle, void*, uint32_t) override {}
    void cmdEndRenderPass(CmdHandle) override {}
    void cmdBindPipeline(CmdHandle, void*) override {}
    void cmdBindDescriptorSets(CmdHandle, void*, void*, void*) override {}
    void cmdBindVertexBuffer(CmdHandle, void*) override {}
    void cmdBindIndexBuffer(CmdHandle, void*, int) override {}
    void cmdPushConstants(CmdHandle, void*, const glm::mat4&) override {}
    void cmdDrawIndexed(CmdHandle,
                        uint32_t, uint32_t, uint32_t, int32_t, uint32_t) override {}

};

/* exported factory */
inline IRenderBackend* createNullBackend() { return new NullBackend(); }

} // namespace gfx
