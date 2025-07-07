#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/catch_approx.hpp>
#include "core/math/Vec.hpp"

using Catch::Approx; 

TEST_CASE("Vec3 arithmetic", "[math]") {
    using core::math::Vec3;

    Vec3 a{1, 2, 3};
    Vec3 b{4, 5, 6};

    auto c = a + b;
    REQUIRE(c[0] == Approx(5));
    REQUIRE(c[1] == Approx(7));
    REQUIRE(c[2] == Approx(9));

    REQUIRE(a.dot(b) == Approx(32));
}
