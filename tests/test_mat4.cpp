#include "core/math/Mat4.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using Catch::Approx;

TEST_CASE("Mat4 identity multiply", "[mat4]") {
    using core::math::Mat4;
    Mat4 a = Mat4::identity();
    Mat4 b = Mat4::identity();

    auto c = a * b;
    REQUIRE(c.m == a.m); // element-wise compare arrays
}

TEST_CASE("Mat4 translation", "[mat4]") {
    using core::math::Mat4;
    using core::math::Vec3;
    Vec3 t{1, 2, 3};
    Mat4 m = Mat4::translate(t);

    REQUIRE(m(3, 0) == Approx(1));
    REQUIRE(m(3, 1) == Approx(2));
    REQUIRE(m(3, 2) == Approx(3));
}
