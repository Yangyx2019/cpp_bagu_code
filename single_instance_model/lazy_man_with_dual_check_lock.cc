#include <mutex>

class Singleton {
public:
    /**
    * 思考：这里有什么问题？
      答：
      1. instance_ != nullptr 并不代表构造完成了。
      有可能会出现编译器先给 instance_内存地址，然后再在这个内存上构造。
      2. 有data race 并发读写
    */
    static Singleton* getInstance() {
        // 第一次检查：避免每次都加锁
        if (instance_ == nullptr) {

            std::lock_guard<std::mutex> lock(mutex_);

            // 第二次检查：防止多个线程重复创建
            if (instance_ == nullptr) {
                instance_ = new Singleton();
            }
        }

        return instance_;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
    ~Singleton() = default;

private:
    static Singleton* instance_;
    static std::mutex mutex_;
};

Singleton* Singleton::instance_ = nullptr;
std::mutex Singleton::mutex_;