#include <cassert>
// #include "hungry_man.h"
#include "lazy_man.h"

Singleton* Singleton::instance_ = nullptr;
std::mutex Singleton::mutex_;

int main() {

    auto s1 = &Singleton::getInstance();
    auto s2 = &Singleton::getInstance();
    assert(s1 == s2);
    // assert(s1->val_ == 2);

    return 0;
}