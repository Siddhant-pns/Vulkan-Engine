#include <catch2/catch_test_macros.hpp>
#include "core/math/Mat3.hpp"
#include <catch2/catch_approx.hpp>

using Catch::Approx; 

TEST_CASE("Mat3 determinant & inverse", "[mat3]") {
    using namespace core::math;
    Mat3 m = Mat3::rotate(Quat::fromAxisAngle({0,0,1}, 1.0f));
    float det = m.determinant();
    REQUIRE(det == Approx(1.0f).margin(1e-5));

    Mat3 inv = m.inverse();
    Mat3 id = m * inv;
    for (std::size_t i = 0; i < 9; ++i) {
        const bool isDiag = (i % 4) == 0;        // 0,4,8
        REQUIRE(id.m[i] == Approx(isDiag ? 1.f : 0.f).margin(1e-4));
    }

}
