#pragma once
#include "Vec.hpp"

namespace core::math {

struct Quat {
    float w{1.f}, x{0.f}, y{0.f}, z{0.f};

    static Quat identity() noexcept { return {}; }

    /* stubs for now */
    static Quat fromAxisAngle(const Vec3& /*axis*/, float /*radians*/) noexcept {
        return {};
    }
    Vec3 rotate(const Vec3& v) const noexcept { return v; }
};

} // namespace core::math
