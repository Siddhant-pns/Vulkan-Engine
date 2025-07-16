#pragma once
#include <cmath>
#include "Vec.hpp"

namespace core::math {

/* left-handed, unit quaternion ----------------------------------*/
struct Quat {
    float w{1.f}, x{0.f}, y{0.f}, z{0.f};

    /* ctors */
    constexpr Quat() = default;
    constexpr Quat(float w_, float x_, float y_, float z_)
        : w(w_), x(x_), y(y_), z(z_) {}

    static Quat identity() { return {}; }

    /* axis–angle (radians) */
    static Quat fromAxisAngle(const Vec3& axis, float rad) {
        float half = rad*0.5f;
        float s = std::sin(half);
        return { std::cos(half), axis[0]*s, axis[1]*s, axis[2]*s };
    }

    /* multiply (composition) */
    Quat operator*(const Quat& b) const noexcept {
        return {
            w*b.w - x*b.x - y*b.y - z*b.z,
            w*b.x + x*b.w + y*b.z - z*b.y,
            w*b.y - x*b.z + y*b.w + z*b.x,
            w*b.z + x*b.y - y*b.x + z*b.w
        };
    }

    /* rotate vec3 */
    Vec3 rotate(const Vec3& v) const noexcept {
        Vec3 qv{x, y, z};
        Vec3 t = qv.cross(v) * 2.f;
        return v + t*w + qv.cross(t);
    }
};

} // namespace core::math
