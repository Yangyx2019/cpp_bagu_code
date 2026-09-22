class Singleton {
public:
    /**
    * 思考：确实是需要的时候再创建实例了，但是有什么新的问题？
    答：
    1. 线程不安全
        有可能并发读写导致data race
        有可能new了两个Singleton然后内存泄露
    */
    static Singleton* getInstance() {
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
};

Singleton* Singleton::instance_ = nullptr;

int main()
{
    Singleton* s1 = Singleton::getInstance();
    Singleton* s2 = Singleton::getInstance();
}