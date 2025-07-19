#pragma once
#define VK_NO_PROTOTYPES
#include <volk.h>

namespace gfx {
struct FrameSync {
    VkSemaphore imageAcquired{VK_NULL_HANDLE};
    VkSemaphore renderFinished{VK_NULL_HANDLE};
    VkFence inFlight{VK_NULL_HANDLE};
    VkCommandBuffer cmd{VK_NULL_HANDLE};
};
} // namespace gfx
