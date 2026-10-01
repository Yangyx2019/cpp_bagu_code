#pragma once
#include <mutex>
class Singleton {
public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Singleton& getInstance() {
        std::lock_guard<std::mutex> lck_guard(mutex_);
        if (instance_ == nullptr) {
            instance_ = new Singleton();
        }
        return *instance_;
    }

private:
    Singleton() {};
    static Singleton* instance_;
    static std::mutex mutex_;
};