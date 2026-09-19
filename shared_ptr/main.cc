#include <iostream>
#include <cassert>
#include "yyx_shared_ptr.h"
using namespace std;
class A {
public:
    ~A() {
        cout << "A is destroyed!!" << endl;
    }
};

void test_cp(shared_ptr<A> sp_a) {
    assert(sp_a.getCnt() == 2);
    std::cout << "in fun" << endl;
}

void test_mv(shared_ptr<A> sp_a) {

}

int main() {
    shared_ptr<A> sp_a(new A);
    assert(sp_a.getCnt() == 1);
    test_cp(sp_a);
    assert(sp_a.getCnt() == 1);
    test_mv(std::move(sp_a));
    assert(sp_a.getPtr() == nullptr);
    assert(sp_a.getCnt() == 0);

}