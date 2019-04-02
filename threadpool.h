#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <iostream>

#define MAX_THREAD_NUM 256

class ThreadPool {
    using Task = std::function<void()>;

    std::vector<std::thread> pool;
    std::queue<Task> tasks;
    std::mutex m_lock;
    std::condition_variable cv_task;
    std::atomic<bool> stoped;
    std::atomic<int> idThrNum;

public:
    ThreadPool(int size = 4);

    template<typename Fun, typename ... Args>
    auto commit(Fun&& fun, Args&&... args) -> std::future<decltype(fun(args...))> {
    if (stoped.load())
        throw std::runtime_error("commit on ThreadPoll is stopped.");

    using RetType = decltype(fun(args...));

    auto task = std::make_shared<std::packaged_task<RetType()>> (
        std::bind(std::forward<Fun>(fun), std::forward<Args>(args)...)
    );

    std::future<RetType> future = task->get_future();
    {
        std::lock_guard<std::mutex> lock{m_lock};
        tasks.emplace(
                    [task]() {
            (*task)();
        }
        );
    }

    cv_task.notify_one();
    return future;
}
    ~ThreadPool();
};


#endif // THREADPOOL_H
