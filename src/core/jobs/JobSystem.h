#pragma once
#include <atomic>
#include <condition_variable>
#include <deque>
#include <future>
#include <thread>
#include <vector>

namespace core::jobs {

/* ---------------------------------------------------------------
   Simple work-stealing job system
   * start()  – spin up N threads (defaults: hwConcurrency-1)
   * stop()   – join all threads (auto-called on exit)
   * submit(fn, args…) → std::future<R>
-----------------------------------------------------------------*/
class JobSystem {
public:
    using Task = std::function<void()>;

    /* Singleton – explicit init/shutdown.  Thread-safe. */
    static void start(std::size_t workerCount = 0);
    static void stop();

    template<typename Fn, typename... Args>
    static auto submit(Fn&& fn, Args&&... args)
        -> std::future<std::invoke_result_t<Fn, Args...>>
    {
        using R = std::invoke_result_t<Fn, Args...>;
        auto taskPtr = std::make_shared<std::packaged_task<R()>>(
            std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));

        std::future<R> fut = taskPtr->get_future();
        enqueue([taskPtr]() { (*taskPtr)(); });
        return fut;
    }

private:
    /* Internal -------------------------------------------------- */
    static void enqueue(Task&& t);
    static Task  stealTask(std::size_t thiefIdx);

    static inline std::vector<std::thread>          s_threads{};
    static inline std::vector<std::deque<Task>>     s_queues{};
    static inline std::condition_variable           s_cv{};
    static inline std::mutex                        s_cvMutex{};
    static inline std::atomic<bool>                 s_running{false};
};

} // namespace core::jobs
