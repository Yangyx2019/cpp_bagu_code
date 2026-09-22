#include <iostream>
#include <cassert>
#include "unique_ptr.h"
using namespace std;

class Base {
public:
    ~Base() {
        cout << "Base 析构了！！" << endl;
    }
};

void test01() {
    unique_ptr<Base> p(new Base);
}

void test02() {
    unique_ptr<Base> p(nullptr);
}

int main() {
    test01();
}