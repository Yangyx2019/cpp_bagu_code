class Singleton {
public:
    /**
    * 完美，甚至可以写析构函数释放拥有的资源，其他的单例处理起来有各种困难
    */
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
    ~Singleton() = default;
};