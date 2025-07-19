#pragma once
#include "backend/include/Handles.h"
#include <string>
#include <variant>

namespace gfx {

/* ----------------------------------------------------------------- *
   A *logical* resource (no API object yet) referenced by graph passes
 * ----------------------------------------------------------------- */
enum class ResourceType { Texture, Buffer };

struct TextureDesc {
    uint32_t width{1}, height{1}, layers{1};
    uint32_t mipLevels{1};
    enum class Format { RGBA8, Depth24 } format{Format::RGBA8};
};

struct BufferDesc {
    uint64_t sizeBytes{0};
};

struct RenderResource {
    std::string name;
    ResourceType type;
    std::variant<TextureDesc, BufferDesc> desc;

    /* backend handle filled during RenderGraph::compile() */
    std::variant<gfx::TextureHandle, gfx::BufferHandle> handle;
};

} // namespace gfx
