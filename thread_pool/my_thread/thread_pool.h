#pragma once
#include <thread>
#include <stdexcept>
#include <vector>
#include <iostream>

class ThreadPool {
public:
    ThreadPool(int thread_num) {
        if (thread_num > kMaxThreadNum) {
            throw std::runtime_error("线程数量超出限制");
        } else {
            for (int i = 0; i < thread_num; ++i) {
                workers_.push_back(
                    std::thread([i]()
                        {
                            // 这么写会有可能在 << 之间发生线程切换
                            // std::cout << "hello idx:" << i << std::endl;
                            const std::string msg = "hello idx:" + std::to_string(i) + "\n";
                            std::cout << msg;

                        }
                    )
                );
            }
        }
    }
    ~ThreadPool() {
        for (int i = 0; i < workers_.size(); ++i) {
            if (workers_[i].joinable()) {
                workers_[i].join();
            }
        }
    }
private:
    constexpr static int kMaxThreadNum = 1000;
    std::vector<std::thread> workers_;
};