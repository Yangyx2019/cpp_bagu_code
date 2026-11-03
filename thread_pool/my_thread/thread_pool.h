#pragma once
#include <thread>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadPool {
public:
    ThreadPool(int thread_num) : stop_(false) {
        if (thread_num > kMaxThreadNum) {
            throw std::runtime_error("线程数量超出限制");
        } else {
            for (int i = 0; i < thread_num; ++i) {
                workers_.emplace_back(
                    [this]()
                        {
                            while (true) {
                                // 这里放在外面是因为task需要在{}的外面做，否则没办法并行
                                std::function<void()> task;
                                {
                                    std::unique_lock<std::mutex> lock(mutex_);
                                    // 条件wait，当条件为false的时候调用wait等待notify
                                    cv_.wait(lock, [this](){
                                        return stop_ || !tasks_.empty();
                                    });

                                    if (stop_ && tasks_.empty()) {
                                        return;
                                    }

                                    task = std::move(tasks_.front());
                                    tasks_.pop();
                                }
                                task();
                            }

                        }
                );
            }
        }
    }
    ~ThreadPool() {
        std::unique_lock<std::mutex> lck(mutex_);
        stop_ = true;
        lck.unlock();
        cv_.notify_all();
        for (int i = 0; i < workers_.size(); ++i) {
            if (workers_[i].joinable()) {
                workers_[i].join();
            }
        }
    }

    void submit(std::function<void()> fc) {
        {
            std::lock_guard<std::mutex> lck(mutex_);
            if (stop_) {
                throw std::runtime_error("submit on stopped ThreadPool");
            }
            tasks_.push(std::move(fc));
        }
        cv_.notify_one();
    }

private:
    constexpr static int kMaxThreadNum = 1000;
    std::vector<std::thread> workers_;
    std::mutex mutex_;
    std::condition_variable cv_;
    std::queue<std::function<void()>> tasks_;
    bool stop_;
};