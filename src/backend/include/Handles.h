#pragma once
#include <cstdint>

namespace gfx {
struct CmdHandle     { void* ptr{nullptr}; };
struct TextureHandle { uint32_t id{0}; };
struct BufferHandle  { uint32_t id{0}; };

constexpr TextureHandle InvalidTexture{0};
constexpr BufferHandle  InvalidBuffer {0};
}  // namespace gfx
