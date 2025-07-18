#include "RenderGraph.h"
#include "backend/RenderDevice.h"
#include <cassert>
#include <iostream>

namespace gfx {

/* ---------------------------------------------------------------- Resources */
RenderResource& RenderGraph::addTexture(const std::string& n,
                                        const TextureDesc& d) {
    int idx = static_cast<int>(m_resources.size());
    m_resourceIndex[n] = idx;
    m_resources.push_back({n, ResourceType::Texture, d});
    return m_resources.back();
}

RenderResource& RenderGraph::addBuffer(const std::string& n,
                                       const BufferDesc& d) {
    int idx = static_cast<int>(m_resources.size());
    m_resourceIndex[n] = idx;
    m_resources.push_back({n, ResourceType::Buffer, d});
    return m_resources.back();
}

/* ---------------------------------------------------------------- Passes */
RenderPass& RenderGraph::addPass(const std::string& n, PassType t,
                                 std::span<const std::string> r,
                                 std::span<const std::string> w,
                                 RenderPass::ExecuteFn fn) {
    int idx = static_cast<int>(m_passes.size());
    m_passes.push_back(RenderPass{
        n,
        t,
        std::vector<std::string>(r.begin(), r.end()),
        std::vector<std::string>(w.begin(), w.end()),
        std::move(fn)
    });
    if (static_cast<int>(m_edges.size()) <= idx) m_edges.resize(idx + 1);
    return m_passes.back();
}

/* ---------------------------------------------------------------- Compile */
void RenderGraph::compile() {
    /* Build edges: pass A → pass B if B reads a resource A writes */
    m_edges.assign(m_passes.size(), {});
    for (auto& res : m_resources) {
        if (res.type == ResourceType::Texture && res.handle.index() == 0) {
            auto tex = gfx::RenderDevice::createTexture(std::get<TextureDesc>(res.desc));
            res.handle = tex;
        } else if (res.type == ResourceType::Buffer && res.handle.index() == 0) {
            auto buf = gfx::RenderDevice::createBuffer(std::get<BufferDesc>(res.desc));
            res.handle = buf;
        }
    }

    /* Topo sort (DFS) */
    m_execOrder.clear();
    std::vector<int> mark(m_passes.size(), 0);
    for (int n = 0; n < (int)m_passes.size(); ++n)
        if (!mark[n]) topoSort(n, m_execOrder, mark);

    core::util::Logger::info("RenderGraph compiled with %zu passes", m_execOrder.size());
}

void RenderGraph::topoSort(int node, std::vector<int>& out, std::vector<int>& mark) {
    mark[node] = 1;
    for (int to : m_edges[node])
        if (!mark[to]) topoSort(to, out, mark);
    out.push_back(node);
}

/* ---------------------------------------------------------------- Execute */
void RenderGraph::execute(uint64_t frame, gfx::CmdHandle cmd)
{
    for (int idx : m_execOrder) {
        auto& p = m_passes[idx];
        core::util::Logger::info("[RG] Pass %s (idx=%d)", p.name.c_str(), idx);

        RenderPass::ExecCtx ctx{frame, this,
                                gfx::RenderDevice::backend(),  // device
                                cmd};                          // cmd handle
        if (p.onExecute) p.onExecute(ctx);
        std::cout << "[RG] Pass " << p.name << " executed." << std::endl;
    }
}


} // namespace gfx
