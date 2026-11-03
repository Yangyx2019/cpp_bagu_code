#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <memory>
#include <utility>

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_num)
        : stop_(false) {

        for (size_t i = 0; i < thread_num; ++i) {
            workers_.emplace_back([this]() {
                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(mutex_);

                        cv_.wait(lock, [this]() {
                            return stop_ || !tasks_.empty();
                        });

                        // 已经停止，并且任务也处理完了
                        if (stop_ && tasks_.empty()) {
                            return;
                        }

                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }

                    // 注意：执行任务时不要持有 mutex
                    task();
                }
            });
        }
    }

    ~ThreadPool() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }

        // 唤醒所有线程，让它们检查 stop_
        cv_.notify_all();

        // 等待所有工作线程退出
        for (auto& worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    template <typename F, typename... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>
    {
        using ReturnType =
            typename std::result_of<F(Args...)>::type;

        // packaged_task 可以把函数执行结果放进 future
        auto task =
            std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(
                    std::forward<F>(f),
                    std::forward<Args>(args)...
                )
            );

        std::future<ReturnType> result = task->get_future();

        {
            std::lock_guard<std::mutex> lock(mutex_);

            if (stop_) {
                throw std::runtime_error(
                    "submit on stopped ThreadPool"
                );
            }

            // tasks_ 只能保存 void()，
            // 所以再套一层 lambda
            tasks_.emplace([task]() {
                (*task)();
            });
        }

        // 有新任务，唤醒一个工作线程
        cv_.notify_one();

        return result;
    }

private:
    // 工作线程
    std::vector<std::thread> workers_;

    // 任务队列
    std::queue<std::function<void()>> tasks_;

    // 保护任务队列和 stop_
    std::mutex mutex_;

    // worker 没任务时阻塞在这里
    std::condition_variable cv_;

    // 线程池是否停止
    bool stop_;
};