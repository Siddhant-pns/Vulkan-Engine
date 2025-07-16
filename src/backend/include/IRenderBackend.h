#pragma once
#include <cstdint>
#include "Handles.h"
#include "graphics/render/RenderResource.h"   // TextureDesc / BufferDesc


namespace gfx {

class IRenderBackend {
public:
    virtual ~IRenderBackend()                                           = default;

    /* lifecycle ---------------------------------------------------- */
    virtual bool init(void* nativeWindowHandle = nullptr)              = 0;
    virtual void shutdown()                                            = 0;

     /* per-frame ------------------------------------------------------------- */
    virtual CmdHandle beginFrame()                        = 0;   // returns cmd
    virtual void      endFrame (CmdHandle)                = 0;

    /* immediate-mode helpers (non-virtual if you prefer) -------------------- */
    virtual void clearColor(CmdHandle,
                            TextureHandle,
                            const float rgba[4])          = 0;
    /* … drawMesh(), dispatchCompute(), etc. will arrive later … */


    /* resources creation---------------------------------------------------- */
    virtual TextureHandle createTexture(const TextureDesc&)            = 0;
    virtual BufferHandle  createBuffer (const BufferDesc&)             = 0;
    virtual void          destroyTexture(TextureHandle)                = 0;
    virtual void          destroyBuffer (BufferHandle)                 = 0;

};

/* factory every plugin must export */
extern "C" IRenderBackend* createBackend();

} // namespace gfx
