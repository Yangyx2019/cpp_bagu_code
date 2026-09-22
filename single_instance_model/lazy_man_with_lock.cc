#include <mutex>

class Singleton {
public:
    /**
    * 思考：加锁了还有什么问题？
    有问题，其实只有第一次new的时候需要加锁，这里每次都加锁有性能损耗。
    */
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