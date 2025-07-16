#pragma once
#include <array>
#include <cstdint>
#include <cstddef>

#if defined(__SSE2__)
    #include <immintrin.h>
#endif

namespace core::math {

/* Minimal 128-bit float4 wrapper – auto-vectorised fallback if SSE2 unavailable */
struct Float4 {
#if defined(__SSE2__)
    using pack = __m128;
    pack v;

    Float4()                              : v(_mm_setzero_ps()) {}
    explicit Float4(float f)              : v(_mm_set1_ps(f)) {}
    Float4(float x, float y, float z, float w) : v(_mm_set_ps(w, z, y, x)) {}
    explicit Float4(const float* p)       : v(_mm_loadu_ps(p)) {}

    void store(float* p)  const { _mm_storeu_ps(p, v); }

    static Float4 load(const float* p)    { return Float4{p}; }

    /* ops */
    Float4 operator+(Float4 b) const { return {_mm_add_ps(v, b.v)}; }
    Float4 operator-(Float4 b) const { return {_mm_sub_ps(v, b.v)}; }
    Float4 operator*(Float4 b) const { return {_mm_mul_ps(v, b.v)}; }

    float sum() const {
        __m128 shuf = _mm_movehdup_ps(v);
        __m128 sums = _mm_add_ps(v, shuf);
        shuf        = _mm_movehl_ps(shuf, sums);
        sums        = _mm_add_ss(sums, shuf);
        return _mm_cvtss_f32(sums);
    }
#else
    std::array<float,4> v{};

    Float4() = default;
    explicit Float4(float f)                       { v = {f,f,f,f}; }
    Float4(float x, float y, float z, float w)     { v = {x,y,z,w}; }
    explicit Float4(const float* p)                { std::copy_n(p,4,v.begin()); }

    void store(float* p)  const { std::copy_n(v.begin(),4,p); }
    static Float4 load(const float* p) { return Float4{p}; }

    Float4 operator+(Float4 b) const { return {v[0]+b.v[0], v[1]+b.v[1], v[2]+b.v[2], v[3]+b.v[3]}; }
    Float4 operator-(Float4 b) const { return {v[0]-b.v[0], v[1]-b.v[1], v[2]-b.v[2], v[3]-b.v[3]}; }
    Float4 operator*(Float4 b) const { return {v[0]*b.v[0], v[1]*b.v[1], v[2]*b.v[2], v[3]*b.v[3]}; }

    float sum() const               { return v[0]+v[1]+v[2]+v[3]; }
#endif
};

} // namespace core::math
