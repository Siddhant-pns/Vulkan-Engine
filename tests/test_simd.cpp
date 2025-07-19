#include "core/math/Simd128.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using Catch::Approx;

TEST_CASE("Float4 basic ops", "[simd]") {
    using core::math::Float4;
    Float4 a{1, 2, 3, 4};
    Float4 b{4, 3, 2, 1};

    auto c = a + b; // {5,5,5,5}
    float buf[4];
    c.store(buf);
    REQUIRE(buf[0] == Approx(5));
    REQUIRE(c.sum() == Approx(20));
}
