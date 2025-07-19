#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace core::memory {

/* Fast “bump-pointer” frame allocator – no individual frees */
class LinearAllocator {
  public:
    LinearAllocator() = default;
    LinearAllocator(void* memory, std::size_t bytes) {
        reset(memory, bytes);
    }

    void reset(void* memory, std::size_t bytes) {
        m_begin = static_cast<std::byte*>(memory);
        m_size = bytes;
        m_offset = 0;
    }

    void* alloc(std::size_t bytes, std::size_t align = alignof(std::max_align_t)) noexcept {
        std::size_t current = reinterpret_cast<std::size_t>(m_begin) + m_offset;
        std::size_t aligned = (current + align - 1) & ~(align - 1);
        std::size_t newOff = aligned - reinterpret_cast<std::size_t>(m_begin) + bytes;
        assert(newOff <= m_size && "LinearAllocator overflow");
        m_offset = newOff;
        return reinterpret_cast<void*>(aligned);
    }

    void clear() noexcept {
        m_offset = 0;
    }

  private:
    std::byte* m_begin{nullptr};
    std::size_t m_size{0};
    std::size_t m_offset{0};
};

} // namespace core::memory
