#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <span>
#include <cstdint>
#include "RenderResource.h"
#include "RenderPass.h"
#include "core/util/Logger.h"

namespace gfx {

/* Very first-cut framegraph -------------------------------------------------- */
class RenderGraph {
public:
    /* Resource & pass creation */
    RenderResource& addTexture(const std::string& name,
                               const TextureDesc& desc);
    RenderResource& addBuffer (const std::string& name,
                               const BufferDesc& desc);

    RenderPass& addPass(const std::string& name, PassType type,
                        std::span<const std::string> reads,
                        std::span<const std::string> writes,
                        RenderPass::ExecuteFn exec);


    RenderResource&       resource(const std::string& n)       { return m_resources[m_resourceIndex.at(n)]; }
    const RenderResource& resource(const std::string& n) const { return m_resources[m_resourceIndex.at(n)]; }

    /* Offline */
    void compile();                 // topological sort + basic validation

    /* Per-frame */
    void execute(uint64_t frame, gfx::CmdHandle cmd);

private:
    void topoSort(int node, std::vector<int>& out, std::vector<int>& mark);

    std::unordered_map<std::string, int>   m_resourceIndex;
    std::vector<RenderResource>            m_resources;

    std::vector<RenderPass>                m_passes;
    std::vector<std::vector<int>>          m_edges;      // pass dependency DAG
    std::vector<int>                       m_execOrder;  // valid after compile()
};

} // namespace gfx
