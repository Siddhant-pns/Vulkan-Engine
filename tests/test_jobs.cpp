#include <catch2/catch_test_macros.hpp>
#include "core/jobs/JobSystem.h"

TEST_CASE("JobSystem submit and wait", "[jobs]") {
    using namespace core::jobs;
    JobSystem::start(2);

    auto f1 = JobSystem::submit([]{ return 21 + 21; });
    auto f2 = JobSystem::submit([]{ std::this_thread::sleep_for(std::chrono::milliseconds(10)); return 7; });

    REQUIRE(f1.get() == 42);
    REQUIRE(f2.get() == 7);

    JobSystem::stop();
}
