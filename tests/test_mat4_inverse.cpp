#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "core/math/Mat4.hpp"

using Catch::Approx; 

TEST_CASE("Mat4 determinant & inverse", "[mat4]") {
    using core::math::Mat4;
    using core::math::Vec3;

    Mat4 m = Mat4::translate({1,2,3}) * Mat4::scale({2,2,2});
    double det = m.determinant();
    REQUIRE(det == Approx(8));          // scaling by 2³

    Mat4 inv = m.inverse();
    Mat4 id  = m * inv;
    for (std::size_t i = 0; i < 16; ++i) {
        const bool isDiag = (i % 5) == 0;        // 0,5,10,15
        REQUIRE(id.m[i] == Approx(isDiag ? 1.f : 0.f).margin(1e-4));
    }
}
