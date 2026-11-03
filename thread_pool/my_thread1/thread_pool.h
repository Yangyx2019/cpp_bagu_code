#pragma once
#include <queue>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <thread>

class ThreadPool {
public:
    ThreadPool() : stop_(false)
    {
        for (uint i = 0; i < kDefaultThreadNum; ++i)
        {
            threads_.emplace_back([this]()
            {
                while (true)
                {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lck(mutex_);
                        // 当判断条件为False的时候会执行wait等待
                        // 当任务队列非空的时候或者stop的时候不需要等待
                        cv_.wait(lck, [this]()
                        {
                           return !task_.empty() || stop_;
                        });

                        if (stop_ && task_.empty())
                        {
                            break;
                        }
                        task = std::move(task_.front());
                        task_.pop();
                    }
                    task();
                }
            });
        }
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lck(mutex_);
            stop_ = true;
        }
        cv_.notify_all();
        for (int i = 0; i < kDefaultThreadNum; ++i)
        {
            if (threads_[i].joinable())
            {
                threads_[i].join();
            }
        }
    }

    void submit(std::function<void()> task)
    {
        std::lock_guard<std::mutex> lck(mutex_);
        if (stop_)
        {
            throw std::runtime_error("在销毁的线程池中提交任务");
        }
        task_.push(std::move(task));
        cv_.notify_one();
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::function<void()>> task_;
    std::vector<std::thread> threads_;
    bool stop_;
    constexpr static uint kDefaultThreadNum = 10;
};
