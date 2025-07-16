#pragma once
#include "backend/include/IRenderBackend.h"
#include "core/util/Logger.h"

namespace gfx {

class NullBackend final : public IRenderBackend {
public:
    bool init(void*) override        { core::util::Logger::info("[Null] init"); return true; }
    CmdHandle beginFrame() override       { core::util::Logger::info("[Null] beginFrame");  return{}; }
    void endFrame(CmdHandle) override         { core::util::Logger::info("[Null] endFrame");}
    void clearColor(CmdHandle, TextureHandle, const float[4]) override {}
    void shutdown() override         { core::util::Logger::info("[Null] shutdown"); }

    TextureHandle createTexture(const TextureDesc&) override { return {}; }
    BufferHandle  createBuffer (const BufferDesc&) override { return {}; }
    void destroyTexture(TextureHandle) override {}
    void destroyBuffer (BufferHandle) override {}

};

/* exported factory */
inline IRenderBackend* createNullBackend() { return new NullBackend(); }

} // namespace gfx
