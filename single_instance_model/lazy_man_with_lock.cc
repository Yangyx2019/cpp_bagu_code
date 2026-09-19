#include <mutex>

class Singleton {
public:
    static Singleton* getInstance() {
        std::lock_guard<std::mutex> lock(mutex_);

        if (instance_ == nullptr) {
            instance_ = new Singleton();
        }

        return instance_;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;

private:
    static Singleton* instance_;
    static std::mutex mutex_;
};

Singleton* Singleton::instance_ = nullptr;
std::mutex Singleton::mutex_;