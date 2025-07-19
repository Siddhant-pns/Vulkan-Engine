#pragma once
#include "Quat.hpp"
#include "Vec.hpp" // Vec2/3/4
#include <array>
#include <cmath>
#include <cstddef>

namespace core::math {

/* --------------------------------------------------------------------------
   4×4 column-major matrix (m[col][row]) – compatible with GLSL, Vulkan, GLM
-----------------------------------------------------------------------------*/
struct Mat4 {
    std::array<float, 16> m{/*identity*/ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};

    /* element access (col, row) ------------------------------------------------*/
    float& operator()(std::size_t c, std::size_t r) noexcept {
        return m[c * 4 + r];
    }
    const float& operator()(std::size_t c, std::size_t r) const noexcept {
        return m[c * 4 + r];
    }

    /* mat × mat ----------------------------------------------------------------*/
    Mat4 operator*(const Mat4& rhs) const noexcept {
        Mat4 out{};
        for (int c = 0; c < 4; ++c)
            for (int r = 0; r < 4; ++r) {
                out(c, r) = (*this)(0, r) * rhs(c, 0) + (*this)(1, r) * rhs(c, 1) + (*this)(2, r) * rhs(c, 2) +
                            (*this)(3, r) * rhs(c, 3);
            }
        return out;
    }

    /* mat × vec4 ---------------------------------------------------------------*/
    Vec4 operator*(const Vec4& v) const noexcept {
        return {v[0] * m[0] + v[1] * m[4] + v[2] * m[8] + v[3] * m[12],
                v[0] * m[1] + v[1] * m[5] + v[2] * m[9] + v[3] * m[13],
                v[0] * m[2] + v[1] * m[6] + v[2] * m[10] + v[3] * m[14],
                v[0] * m[3] + v[1] * m[7] + v[2] * m[11] + v[3] * m[15]};
    }

    /* factories ----------------------------------------------------------------*/
    static Mat4 identity() {
        return {};
    }

    static Mat4 translate(const Vec3& t) {
        Mat4 r{};
        r(3, 0) = t[0];
        r(3, 1) = t[1];
        r(3, 2) = t[2];
        return r;
    }

    static Mat4 scale(const Vec3& s) {
        Mat4 r{};
        r(0, 0) = s[0];
        r(1, 1) = s[1];
        r(2, 2) = s[2];
        return r;
    }

    static Mat4 rotate(const Quat& q); // declared below
    float determinant() const noexcept;
    Mat4 inverse() const noexcept;
    Mat4 inverseFast() const noexcept; // SIMD-accelerated (falls back if no SIMD)

    static Mat4 perspective(float fovyRad, float aspect, float zNear, float zFar) {
        float f = 1.f / std::tan(fovyRad * 0.5f);
        Mat4 r{};
        r(0, 0) = f / aspect;
        r(1, 1) = f;
        r(2, 2) = zFar / (zNear - zFar);
        r(2, 3) = -1.f;
        r(3, 2) = (zFar * zNear) / (zNear - zFar);
        r(3, 3) = 0.f;
        return r;
    }

    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
        Vec3 f = (center - eye).normalized();
        Vec3 s = f.cross(up).normalized();
        Vec3 u = s.cross(f);

        Mat4 r{};
        r(0, 0) = s[0];
        r(1, 0) = s[1];
        r(2, 0) = s[2];
        r(0, 1) = u[0];
        r(1, 1) = u[1];
        r(2, 1) = u[2];
        r(0, 2) = -f[0];
        r(1, 2) = -f[1];
        r(2, 2) = -f[2];
        r(3, 0) = -s.dot(eye);
        r(3, 1) = -u.dot(eye);
        r(3, 2) = f.dot(eye);
        return r;
    }
};

/* -------- Quat→Mat4 ----------------------------------------------------------*/
inline Mat4 Mat4::rotate(const Quat& q) {
    const float w = q.w, x = q.x, y = q.y, z = q.z;
    const float xx = x * x, yy = y * y, zz = z * z;
    const float xy = x * y, xz = x * z, yz = y * z;
    const float wx = w * x, wy = w * y, wz = w * z;

    Mat4 r{};
    r(0, 0) = 1 - 2 * (yy + zz);
    r(1, 0) = 2 * (xy + wz);
    r(2, 0) = 2 * (xz - wy);
    r(0, 1) = 2 * (xy - wz);
    r(1, 1) = 1 - 2 * (xx + zz);
    r(2, 1) = 2 * (yz + wx);
    r(0, 2) = 2 * (xz + wy);
    r(1, 2) = 2 * (yz - wx);
    r(2, 2) = 1 - 2 * (xx + yy);
    return r;
}

/* determinant ----------------------------------------------------*/
inline float Mat4::determinant() const noexcept {
    const auto& a = m;
    float subfactor00 = a[10] * a[15] - a[11] * a[14];
    float subfactor01 = a[9] * a[15] - a[11] * a[13];
    float subfactor02 = a[9] * a[14] - a[10] * a[13];
    float subfactor03 = a[8] * a[15] - a[11] * a[12];
    float subfactor04 = a[8] * a[14] - a[10] * a[12];
    float subfactor05 = a[8] * a[13] - a[9] * a[12];

    return a[0] * (a[5] * subfactor00 - a[6] * subfactor01 + a[7] * subfactor02) -
           a[1] * (a[4] * subfactor00 - a[6] * subfactor03 + a[7] * subfactor04) +
           a[2] * (a[4] * subfactor01 - a[5] * subfactor03 + a[7] * subfactor05) -
           a[3] * (a[4] * subfactor02 - a[5] * subfactor04 + a[6] * subfactor05);
}

/* inverse (Gauss–Jordan, unrolled) -------------------------------*/
inline Mat4 Mat4::inverse() const noexcept {
    const auto& a = m;
    Mat4 inv;

    inv.m[0] = a[5] * a[10] * a[15] - a[5] * a[11] * a[14] - a[9] * a[6] * a[15] + a[9] * a[7] * a[14] +
               a[13] * a[6] * a[11] - a[13] * a[7] * a[10];
    inv.m[4] = -a[4] * a[10] * a[15] + a[4] * a[11] * a[14] + a[8] * a[6] * a[15] - a[8] * a[7] * a[14] -
               a[12] * a[6] * a[11] + a[12] * a[7] * a[10];
    inv.m[8] = a[4] * a[9] * a[15] - a[4] * a[11] * a[13] - a[8] * a[5] * a[15] + a[8] * a[7] * a[13] +
               a[12] * a[5] * a[11] - a[12] * a[7] * a[9];
    inv.m[12] = -a[4] * a[9] * a[14] + a[4] * a[10] * a[13] + a[8] * a[5] * a[14] - a[8] * a[6] * a[13] -
                a[12] * a[5] * a[10] + a[12] * a[6] * a[9];
    inv.m[1] = -a[1] * a[10] * a[15] + a[1] * a[11] * a[14] + a[9] * a[2] * a[15] - a[9] * a[3] * a[14] -
               a[13] * a[2] * a[11] + a[13] * a[3] * a[10];
    inv.m[5] = a[0] * a[10] * a[15] - a[0] * a[11] * a[14] - a[8] * a[2] * a[15] + a[8] * a[3] * a[14] +
               a[12] * a[2] * a[11] - a[12] * a[3] * a[10];
    inv.m[9] = -a[0] * a[9] * a[15] + a[0] * a[11] * a[13] + a[8] * a[1] * a[15] - a[8] * a[3] * a[13] -
               a[12] * a[1] * a[11] + a[12] * a[3] * a[9];
    inv.m[13] = a[0] * a[9] * a[14] - a[0] * a[10] * a[13] - a[8] * a[1] * a[14] + a[8] * a[2] * a[13] +
                a[12] * a[1] * a[10] - a[12] * a[2] * a[9];
    inv.m[2] = a[1] * a[6] * a[15] - a[1] * a[7] * a[14] - a[5] * a[2] * a[15] + a[5] * a[3] * a[14] +
               a[13] * a[2] * a[7] - a[13] * a[3] * a[6];
    inv.m[6] = -a[0] * a[6] * a[15] + a[0] * a[7] * a[14] + a[4] * a[2] * a[15] - a[4] * a[3] * a[14] -
               a[12] * a[2] * a[7] + a[12] * a[3] * a[6];
    inv.m[10] = a[0] * a[5] * a[15] - a[0] * a[7] * a[13] - a[4] * a[1] * a[15] + a[4] * a[3] * a[13] +
                a[12] * a[1] * a[7] - a[12] * a[3] * a[5];
    inv.m[14] = -a[0] * a[5] * a[14] + a[0] * a[6] * a[13] + a[4] * a[1] * a[14] - a[4] * a[2] * a[13] -
                a[12] * a[1] * a[6] + a[12] * a[2] * a[5];
    inv.m[3] = -a[1] * a[6] * a[11] + a[1] * a[7] * a[10] + a[5] * a[2] * a[11] - a[5] * a[3] * a[10] -
               a[9] * a[2] * a[7] + a[9] * a[3] * a[6];
    inv.m[7] = a[0] * a[6] * a[11] - a[0] * a[7] * a[10] - a[4] * a[2] * a[11] + a[4] * a[3] * a[10] +
               a[8] * a[2] * a[7] - a[8] * a[3] * a[6];
    inv.m[11] = -a[0] * a[5] * a[11] + a[0] * a[7] * a[9] + a[4] * a[1] * a[11] - a[4] * a[3] * a[9] -
                a[8] * a[1] * a[7] + a[8] * a[3] * a[5];
    inv.m[15] = a[0] * a[5] * a[10] - a[0] * a[6] * a[9] - a[4] * a[1] * a[10] + a[4] * a[2] * a[9] +
                a[8] * a[1] * a[6] - a[8] * a[2] * a[5];

    float det = a[0] * inv.m[0] + a[1] * inv.m[4] + a[2] * inv.m[8] + a[3] * inv.m[12];
    float invDet = 1.0f / det;
    for (float& f : inv.m)
        f *= invDet;
    return inv;
}

/* ---------------------------------------------------------------------------
   Fast inverse using SSE (float4 ops); returns inverse() if SIMD not available
----------------------------------------------------------------------------*/
inline Mat4 Mat4::inverseFast() const noexcept {
#if defined(__SSE2__)
    using namespace core::math;
    const __m128 row0 = _mm_loadu_ps(&m[0]);
    const __m128 row1 = _mm_loadu_ps(&m[4]);
    const __m128 row2 = _mm_loadu_ps(&m[8]);
    const __m128 row3 = _mm_loadu_ps(&m[12]);

    /* cf. Intel “Optimizing 4x4 Matrix Inverse” – unrolled variant */
    __m128 fac0 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 3, 0, 0)),
                                        _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(2, 2, 1, 1))),
                             _mm_mul_ps(_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 2, 1, 1)),
                                        _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(3, 3, 0, 0))));

    __m128 fac1 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 3, 1, 1)),
                                        _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(2, 2, 0, 0))),
                             _mm_mul_ps(_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 2, 0, 0)),
                                        _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(3, 3, 1, 1))));

    __m128 fac2 = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(2, 2, 0, 0)),
                                        _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(3, 3, 1, 1))),
                             _mm_mul_ps(_mm_shuffle_ps(row2, row2, _MM_SHUFFLE(3, 3, 1, 1)),
                                        _mm_shuffle_ps(row3, row3, _MM_SHUFFLE(2, 2, 0, 0))));

    __m128 vec0 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(0, 0, 0, 0));
    __m128 vec1 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(1, 1, 1, 1));
    __m128 vec2 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(2, 2, 2, 2));
    __m128 vec3 = _mm_shuffle_ps(row1, row1, _MM_SHUFFLE(3, 3, 3, 3));

    __m128 inv0 = _mm_add_ps(_mm_mul_ps(vec1, fac0), _mm_sub_ps(_mm_mul_ps(vec2, fac1), _mm_mul_ps(vec3, fac2)));

    vec0 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(0, 0, 0, 0));
    vec1 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(1, 1, 1, 1));
    vec2 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(2, 2, 2, 2));
    vec3 = _mm_shuffle_ps(row0, row0, _MM_SHUFFLE(3, 3, 3, 3));

    __m128 inv1 = _mm_add_ps(_mm_mul_ps(vec0, fac0), _mm_sub_ps(_mm_mul_ps(vec2, fac2), _mm_mul_ps(vec3, fac1)));

    __m128 inv2 = _mm_add_ps(_mm_mul_ps(vec0, fac1), _mm_sub_ps(_mm_mul_ps(vec1, fac2), _mm_mul_ps(vec3, fac0)));

    __m128 inv3 = _mm_add_ps(_mm_mul_ps(vec0, fac2), _mm_sub_ps(_mm_mul_ps(vec1, fac1), _mm_mul_ps(vec2, fac0)));

    /* determinant */
    __m128 signA = _mm_set_ps(-0.f, 0.f, -0.f, 0.f);
    __m128 dot0 = _mm_mul_ps(row0, inv0);
    dot0 = _mm_hadd_ps(dot0, dot0);
    dot0 = _mm_hadd_ps(dot0, dot0);
    __m128 det = _mm_div_ps(_mm_set1_ps(1.f), dot0);

    /* transpose & multiply by reciprocal det */
    __m128 invt0 = _mm_mul_ps(_mm_xor_ps(inv0, signA), det);
    __m128 invt1 = _mm_mul_ps(_mm_xor_ps(inv1, signA), det);
    __m128 invt2 = _mm_mul_ps(_mm_xor_ps(inv2, signA), det);
    __m128 invt3 = _mm_mul_ps(_mm_xor_ps(inv3, signA), det);

    Mat4 r;
    _mm_storeu_ps(&r.m[0], _mm_unpacklo_ps(invt0, invt1));
    _mm_storeu_ps(&r.m[4], _mm_unpackhi_ps(invt0, invt1));
    _mm_storeu_ps(&r.m[8], _mm_unpacklo_ps(invt2, invt3));
    _mm_storeu_ps(&r.m[12], _mm_unpackhi_ps(invt2, invt3));
    return r;
#else
    return inverse(); // scalar fallback
#endif
}

} // namespace core::math
