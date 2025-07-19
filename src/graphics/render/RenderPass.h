#pragma once
#include "backend/include/IRenderBackend.h"
#include <functional>
#include <string>
#include <vector>

namespace gfx {

struct RenderResource;

enum class PassType { Graphics, Compute, Copy };

/* A node in the graph: declares reads/writes + lambda to record work */
struct RenderPass {
    std::string name;
    PassType type{PassType::Graphics};
    std::vector<std::string> reads;
    std::vector<std::string> writes;

    /* Callback invoked at execution time.
       Args: this pass’ index, user-opaque context ptr (later), frame number. */
    struct ExecCtx { // provided at execution
        uint64_t frame;
        class RenderGraph* graph; // query handles if needed
        gfx::IRenderBackend* device;
        gfx::CmdHandle cmd;
    };
    using ExecuteFn = std::function<void(const ExecCtx&)>;
    ExecuteFn onExecute;
};

} // namespace gfx
