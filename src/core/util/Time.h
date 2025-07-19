#pragma once
#include <chrono>

namespace core::util {

/* Simple frame timer --------------------------------------------- */
class Time {
  public:
    using Clock = std::chrono::steady_clock;
    using SecondsF64 = std::chrono::duration<double>;

    /* Call once at app start. */
    static void init() noexcept {
        s_start = Clock::now();
        s_prev = s_start;
        s_delta = SecondsF64{0};
    }

    /* Call once per frame (or manually). */
    static void tick() noexcept {
        const auto now = Clock::now();
        s_delta = now - s_prev;
        s_prev = now;
    }

    /* Seconds since last tick() */
    static double delta() noexcept {
        return s_delta.count();
    }

    /* Seconds since init() */
    static double total() noexcept {
        return SecondsF64{Clock::now() - s_start}.count();
    }

  private:
    inline static Clock::time_point s_start{};
    inline static Clock::time_point s_prev{};
    inline static SecondsF64 s_delta{};
};

} // namespace core::util
