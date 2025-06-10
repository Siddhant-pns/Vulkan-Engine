#pragma once

namespace renderer {
    class IRenderer {
    public:
        virtual void Init(void* windowPtr) = 0;
        virtual void BeginFrame() = 0;
        virtual void EndFrame() = 0;
        virtual void Cleanup() = 0;
        virtual ~IRenderer() = default;
    };
}
