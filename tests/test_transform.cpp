#include <catch2/catch_test_macros.hpp>
#include "core/math/Transform.hpp"
#include <catch2/catch_approx.hpp>

using Catch::Approx; 

TEST_CASE("composeTRS translation", "[transform]") {
    using namespace core::math;
    Mat4 m = composeTRS({3,4,5}, Quat::identity(), {1,1,1});
    REQUIRE(m(3,0) == Approx(3));
    REQUIRE(m(3,1) == Approx(4));
    REQUIRE(m(3,2) == Approx(5));
}
