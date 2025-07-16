#include <catch2/catch_test_macros.hpp>
#include "core/math/Mat4.hpp"
#include <catch2/catch_approx.hpp>

using Catch::Approx; 

TEST_CASE("inverseFast matches scalar", "[mat4_simd]") {
    using namespace core::math;
    Mat4 m = Mat4::translate({1,2,3}) * Mat4::rotate(Quat::fromAxisAngle({0,1,0}, 0.8f))
           * Mat4::scale({0.5f,2.0f,1.5f});

    Mat4 a = m.inverseFast();
    Mat4 b = m.inverse();

    for (std::size_t i=0;i<16;++i)
        REQUIRE(a.m[i] == Approx(b.m[i]).margin(1e-4));
}
