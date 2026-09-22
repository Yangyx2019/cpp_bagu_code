#include <iostream>

class Singleton {
public:
    static Singleton& getInstance() {
        return instance_;
    }
    /**
    * 思考：
        1. 为什么需要delete拷贝操作？
        答：单例不允许拷贝，如果允许就会出现两个实例

        2. 为什么没有delete移动操作？
        答：因为显式指定了拷贝操作，所以编译器不会自动生成移动构造和移动赋值，所以其实移动操作也没了

        3. 为什么需要delete移动操作呢？单例不允许移动吗？移动了不还是单例吗？
        答：不允许，移动并不是说被移动的实例就不存在了，只不过是可能不占资源或者是一个初始状态，这还是违反了单例

        4. 什么是 Rule of 5 和 Rule of Zero？
        5指的是移动构造移动赋值 拷贝构造拷贝赋值 还有析构函数
        如果自己实现了以上任意一个函数，程序员就应该自己实现其他四个函数。
        所以写了以上5个函数中的1个就要写其他四个，这就是Rule of 5.
        Attention！！
        通常来说，对拷贝构造和拷贝赋值做了delete，编译器不会生成移动构造和移动赋值，但是默认的析构函数可能还是有的。
        5. 有没有线程安全问题
        对于当前例子没有

        6. 有什么不好的地方？
        答：饿汉式，无论是否需要这个单例都一定会创造，如果实例很大就不好。
        */
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() {
        std::cout << "Singleton constructed\n";
    }

    ~Singleton() = default;

private:
    static Singleton instance_;
};

// 类外定义
Singleton Singleton::instance_;

int main() {
    Singleton& s1 = Singleton::getInstance();
    Singleton& s2 = Singleton::getInstance();

    std::cout << (&s1 == &s2) << '\n';  // 1
}