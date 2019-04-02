#include "threadpool.h"


ThreadPool::ThreadPool(int size)
    :stoped(false)
{
    idThrNum = size < 1 ? 1: size;
    for (int i = 0; i < idThrNum; ++i) {
        pool.emplace_back(
            [this]() {
                while(!this->stoped) {
                    Task task;
                    {
                        std::unique_lock<std::mutex> lock{this->m_lock};
                        this->cv_task.wait(lock, [this] {return this->stoped.load() or !this->tasks.empty();});
                        if (this->stoped.load() and this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }
                    --idThrNum;
                    task();
                    ++idThrNum;
                }
            }
        );
    }
}


ThreadPool::~ThreadPool() {
    stoped.store(true);
    cv_task.notify_all();
    for(std::thread& thread : pool) {
        if(thread.joinable())
            thread.join();;
    }
}

