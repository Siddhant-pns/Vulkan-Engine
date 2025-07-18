#pragma once
#include <cstdint>
#include "Handles.h"
#include "graphics/render/RenderResource.h"   // TextureDesc / BufferDesc
#include "glm/glm.hpp"  // for glm::mat4


namespace gfx {

class IRenderBackend {
public:
    virtual ~IRenderBackend()                                           = default;

    /* lifecycle ---------------------------------------------------- */
    virtual bool init(void* nativeWindowHandle = nullptr)              = 0;
    virtual void shutdown()                                            = 0;
    virtual void preShutdown()                                         = 0;   // optional, before shutdown() 

     /* per-frame ------------------------------------------------------------- */
    virtual gfx::CmdHandle beginFrame()                        = 0;   // returns cmd
    virtual void      endFrame (gfx::CmdHandle)                = 0;

    /* immediate-mode helpers (non-virtual if you prefer) -------------------- */
    virtual void clearColor(gfx::CmdHandle,
                            TextureHandle,
                            const float rgba[4])          = 0;
    /* … drawMesh(), dispatchCompute(), etc. will arrive later … */


    /* resources creation---------------------------------------------------- */
    virtual TextureHandle createTexture(const TextureDesc&)            = 0;
    virtual BufferHandle  createBuffer (const BufferDesc&)             = 0;
    virtual void          destroyTexture(TextureHandle)                = 0;
    virtual void          destroyBuffer (BufferHandle)                 = 0;


    virtual void cmdBeginRenderPass(CmdHandle, void* pipeline, uint32_t fbIdx) = 0;
    virtual void cmdEndRenderPass(CmdHandle) = 0;
    virtual void cmdBindPipeline(CmdHandle, void* pipeline) = 0;
    virtual void cmdBindDescriptorSets(CmdHandle, void* layout,
                                    void* set0, void* set1) = 0;
    virtual void cmdBindVertexBuffer(CmdHandle, void* buffer) = 0;
    virtual void cmdBindIndexBuffer(CmdHandle, void* buffer, int indexType) = 0;
    virtual void cmdPushConstants(CmdHandle, void* layout, const glm::mat4&) = 0;
    virtual void cmdDrawIndexed(CmdHandle,
                                uint32_t idxCnt, uint32_t instCnt,
                                uint32_t firstIdx, int32_t vtxOffset,
                                uint32_t firstInst) = 0;

};

/* factory every plugin must export */
extern "C" IRenderBackend* createBackend();

} // namespace gfx
