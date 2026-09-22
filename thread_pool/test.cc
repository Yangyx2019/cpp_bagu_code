#include <iostream>
#include "ThreadPool.h"

int add(int a, int b) {
    return a + b;
}

int main() {
    ThreadPool pool(4);

    auto f1 = pool.submit(add, 1, 2);

    auto f2 = pool.submit([](int x) {
        return x * x;
    }, 10);

    pool.submit([]() {
        std::cout << "hello thread pool\n";
    });

    std::cout << f1.get() << std::endl; // 3
    std::cout << f2.get() << std::endl; // 100

    return 0;
}