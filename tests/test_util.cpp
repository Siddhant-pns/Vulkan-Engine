#include <catch2/catch_test_macros.hpp>
#include "core/util/Time.h"
#include "core/util/FileSystem.h"

TEST_CASE("Time tick", "[util]") {
    using core::util::Time;
    Time::init();
    Time::tick();
    REQUIRE(Time::delta() >= 0.0);        // should be small but non-negative
}

TEST_CASE("FileSystem exists", "[util]") {
    using core::util::FileSystem;
    REQUIRE(FileSystem::exists(std::filesystem::current_path()));
}
