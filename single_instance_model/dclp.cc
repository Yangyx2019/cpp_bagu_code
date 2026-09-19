#include <atomic>
#include <mutex>

class Singleton {
public:
    static Singleton* getInstance() {
        Singleton* tmp =
            instance_.load(std::memory_order_acquire);

        // 第一次检查
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);

            tmp = instance_.load(std::memory_order_relaxed);

            // 第二次检查
            if (tmp == nullptr) {
                tmp = new Singleton();

                instance_.store(
                    tmp,
                    std::memory_order_release
                );
            }
        }

        return tmp;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;

private:
    static std::atomic<Singleton*> instance_;
    static std::mutex mutex_;
};

std::atomic<Singleton*> Singleton::instance_{nullptr};
std::mutex Singleton::mutex_;