void print(int x, const std::string& s) {
    std::cout << x << " " << s << std::endl;
}

int add(int a, int b) {
    std::cout << a + b << std::endl;
    return a + b;
}

int main() {
    Timer timer;

    // 普通函数 + 多个参数
    timer.scheduleAfter(
        std::chrono::milliseconds(1000),
        print,
        100,
        std::string("hello")
    );

    // lambda + 参数
    timer.scheduleAfter(
        std::chrono::milliseconds(2000),
        [](int a, int b) {
            std::cout << a * b << std::endl;
        },
        3,
        4
    );

    // 返回值可以有，只是 Timer 不关心返回值
    auto id = timer.scheduleAfter(
        std::chrono::milliseconds(3000),
        add,
        10,
        20
    );

    // timer.cancel(id);

    std::this_thread::sleep_for(
        std::chrono::seconds(5)
    );
}