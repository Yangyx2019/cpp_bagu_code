class Singleton {
public:
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