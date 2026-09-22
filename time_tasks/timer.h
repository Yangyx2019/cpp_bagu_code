#include <iostream>
#include <map>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Timer {
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Task = std::function<void()>;
    using TaskId = uint64_t;

private:
    struct TimerTask {
        TaskId id;
        Task task;
    };

    using TimerMap = std::multimap<TimePoint, TimerTask>;

public:
    Timer() : stop_(false), next_id_(0) {
        worker_ = std::thread([this]() {
            run();
        });
    }

    ~Timer() {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            stop_ = true;
        }

        cv_.notify_all();

        if (worker_.joinable()) {
            worker_.join();
        }
    }

    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;

    // 支持任意函数 + 任意参数
    template<typename F, typename... Args>
    TaskId scheduleAfter(
        std::chrono::milliseconds delay,
        F&& f,
        Args&&... args) {

        // 把函数 + 参数绑定成一个 void() 任务
        Task task = std::bind(
            std::forward<F>(f),
            std::forward<Args>(args)...
        );

        TimePoint expire = Clock::now() + delay;

        std::lock_guard<std::mutex> lock(mutex_);

        TaskId id = next_id_++;

        auto it = tasks_.emplace(
            expire,
            TimerTask{id, std::move(task)}
        );

        index_[id] = it;

        cv_.notify_one();

        return id;
    }

    bool cancel(TaskId id) {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = index_.find(id);

        if (it == index_.end()) {
            return false;
        }

        tasks_.erase(it->second);
        index_.erase(it);

        cv_.notify_one();

        return true;
    }

private:
    void run() {
        std::unique_lock<std::mutex> lock(mutex_);

        while (!stop_) {

            // 没任务就等待
            if (tasks_.empty()) {
                cv_.wait(lock, [this]() {
                    return stop_ || !tasks_.empty();
                });

                continue;
            }

            // 最近到期的任务
            auto it = tasks_.begin();

            // 还没到时间
            if (Clock::now() < it->first) {
                cv_.wait_until(lock, it->first);
                continue;
            }

            // 到期了
            TaskId id = it->second.id;
            Task task = std::move(it->second.task);

            index_.erase(id);
            tasks_.erase(it);

            // 执行用户任务时不能持锁
            lock.unlock();

            task();

            lock.lock();
        }
    }

private:
    TimerMap tasks_;

    std::unordered_map<TaskId, TimerMap::iterator> index_;

    std::mutex mutex_;
    std::condition_variable cv_;

    std::thread worker_;

    bool stop_;
    TaskId next_id_;
};