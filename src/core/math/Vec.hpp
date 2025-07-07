#pragma once
#include <array>
#include <cmath>
#include <cstddef>

namespace core::math {

template<typename T, std::size_t N>
struct Vec {
    static_assert(N > 0 && N <= 4);

    /* data */
    std::array<T, N> v{};

    /* ctors */
    constexpr Vec() = default;
    constexpr Vec(std::initializer_list<T> list) {
        std::size_t i = 0;
        for (T val : list) { if (i < N) v[i++] = val; }
    }

    /* element access */
    constexpr T&       operator[](std::size_t i)       noexcept { return v[i]; }
    constexpr const T& operator[](std::size_t i) const noexcept { return v[i]; }

    /* arithmetic */
    constexpr Vec  operator+(const Vec& rhs) const noexcept {
        Vec r;  for (std::size_t i = 0; i < N; ++i) r[i] = v[i] + rhs[i];  return r;
    }
    constexpr Vec  operator-(const Vec& rhs) const noexcept {
        Vec r;  for (std::size_t i = 0; i < N; ++i) r[i] = v[i] - rhs[i];  return r;
    }
    constexpr Vec  operator*(T s)           const noexcept {
        Vec r;  for (std::size_t i = 0; i < N; ++i) r[i] = v[i] * s;       return r;
    }
    constexpr Vec& operator+=(const Vec& rhs)          noexcept { return *this = *this + rhs; }
    constexpr Vec& operator-=(const Vec& rhs)          noexcept { return *this = *this - rhs; }
    constexpr Vec& operator*=(T s)                     noexcept { return *this = *this * s;   }

    /* metrics */
    constexpr T dot(const Vec& rhs) const noexcept {
        T r{};  for (std::size_t i = 0; i < N; ++i) r += v[i] * rhs[i];  return r;
    }
    constexpr T lengthSquared() const noexcept { return dot(*this); }
    T           length()        const noexcept { return std::sqrt(lengthSquared()); }
    Vec         normalized()    const noexcept { return *this * (T(1) / length()); }
};

/* convenient aliases */
using Vec2 = Vec<float, 2>;
using Vec3 = Vec<float, 3>;
using Vec4 = Vec<float, 4>;

} // namespace core::math
