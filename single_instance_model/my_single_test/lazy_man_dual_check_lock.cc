#include <mutex>
#include <cassert>
#include <atomic>
class Singleton {
public:
    static Singleton& getInstance() {
        if (!isOk_) { // 注意这里判断的时候不能加instance_ == nullptr 否则会有data race
            std::lock_guard<std::mutex> guard(mutex_);
            if (instance_ == nullptr) {
                instance_ = new Singleton();
                isOk_ = true;
            }
        }
        return *instance_;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
private:
    Singleton() = default;
    static std::mutex mutex_;
    static std::atomic<bool> isOk_; 
    static Singleton* instance_;
};

std::mutex Singleton:: mutex_;
std::atomic<bool> Singleton::isOk_ = false;
Singleton* Singleton::instance_ = nullptr;

int main() {
    auto& s1 = Singleton::getInstance();
    auto& s2 = Singleton::getInstance();
    assert(&s1 == &s2);
}