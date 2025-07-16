#include "core/jobs/JobSystem.h"
#include "core/util/Logger.h"
#include <random>

namespace core::jobs {

/* -------------------- public ---------------------------------- */
void JobSystem::start(std::size_t workerCount)
{
    if (s_running.exchange(true))
        return;                           // already running

    if (workerCount == 0)
        workerCount = std::max(1u, std::thread::hardware_concurrency() - 1);

    s_queues.resize(workerCount);
    s_threads.reserve(workerCount);

    for (std::size_t i = 0; i < workerCount; ++i) {
        s_threads.emplace_back([i] {
            std::mt19937 rng{static_cast<unsigned>(i)};
            std::uniform_int_distribution<std::size_t> dist(0, s_queues.size() - 1);

            while (s_running.load(std::memory_order_relaxed)) {
                Task t;

                /* try own deque first */
                if (!s_queues[i].empty()) {
                    t = std::move(s_queues[i].front());
                    s_queues[i].pop_front();
                } else {
                    /* steal from random victim */
                    std::size_t victim = dist(rng);
                    if (victim != i && !s_queues[victim].empty()) {
                        t = std::move(s_queues[victim].back());
                        s_queues[victim].pop_back();
                    }
                }

                if (t) {
                    t();
                } else {
                    /* sleep until new work arrives */
                    std::unique_lock lk{s_cvMutex};
                    s_cv.wait_for(lk, std::chrono::milliseconds(1));
                }
            }
        });
    }

    core::util::Logger::info("JobSystem started with %zu workers", workerCount);
}

void JobSystem::stop()
{
    if (!s_running.exchange(false)) return;
    s_cv.notify_all();
    for (auto& th : s_threads) if (th.joinable()) th.join();
    s_threads.clear();
    s_queues.clear();
}

void JobSystem::enqueue(Task&& t)
{
    /* push onto the smallest queue – trivial load balancing */
    auto it = std::min_element(
        s_queues.begin(), s_queues.end(),
        [](auto& a, auto& b){ return a.size() < b.size(); });

    it->push_back(std::move(t));
    s_cv.notify_one();
}

} // namespace core::jobs
