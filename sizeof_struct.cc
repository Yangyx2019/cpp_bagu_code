#include <cstddef>
#include <iostream>

// 结构体内存对齐练习题整理
// 常见 64 位平台下假设：
// sizeof(char)   = 1, alignof(char)   = 1
// sizeof(int)    = 4, alignof(int)    = 4
// sizeof(double) = 8, alignof(double) = 8
// 注意：很多常见内置类型中，sizeof(T) 和 alignof(T) 恰好相同，
// 例如常见平台上的 int 为 4/4、double 为 8/8，
// 但这不是 C++ 规定的普遍规律，不能认为 sizeof(T) == alignof(T)。

// 对普通结构体而言，在没有 alignas、#pragma pack 等特殊设置时，
// alignof(struct) 通常等于其成员中最大的 alignof 值。

// alignof(T) 的含义：T 类型对象的起始地址必须满足该类型的对齐要求。
// 可以直观理解为：address % alignof(T) == 0。

// 题 1
struct A1 {
    char c;
    int i;
};

// 预期：
// sizeof(A1)  = 8
// alignof(A1) = 4
// offsetof(c) = 0
// offsetof(i) = 4


// 题 2
struct B1 {
    char c1;
    char c2;
    int i;
};

// 预期：
// sizeof(B1)  = 8
// alignof(B1) = 4
// offsetof(c1) = 0
// offsetof(c2) = 1
// offsetof(i)  = 4


// 题 3
struct C1 {
    char c;
    double d;
    int i;
};

// 预期：
// sizeof(C1)  = 24
// alignof(C1) = 8
// offsetof(c) = 0
// offsetof(d) = 8
// offsetof(i) = 16


// 题 4
struct D1 {
    double d;
    char c;
    int i;
};

// 预期：
// sizeof(D1)  = 16
// alignof(D1) = 8
// offsetof(d) = 0
// offsetof(c) = 8
// offsetof(i) = 12


// 题 5：嵌套结构体
struct InnerA {
    char c;
    int i;
};

// sizeof(InnerA)  = 8
// alignof(InnerA) = 4

struct OuterB {
    char x;
    InnerA a;
    char y;
};

// 预期：
// sizeof(OuterB)  = 16
// alignof(OuterB) = 4
// offsetof(x) = 0
// offsetof(a) = 4
// offsetof(y) = 12


int main() {
    std::cout << "A1:\n";
    std::cout << "  sizeof  = " << sizeof(A1) << '\n';
    std::cout << "  alignof = " << alignof(A1) << '\n';
    std::cout << "  offset c = " << offsetof(A1, c) << '\n';
    std::cout << "  offset i = " << offsetof(A1, i) << "\n\n";

    std::cout << "B1:\n";
    std::cout << "  sizeof  = " << sizeof(B1) << '\n';
    std::cout << "  alignof = " << alignof(B1) << '\n';
    std::cout << "  offset c1 = " << offsetof(B1, c1) << '\n';
    std::cout << "  offset c2 = " << offsetof(B1, c2) << '\n';
    std::cout << "  offset i  = " << offsetof(B1, i) << "\n\n";

    std::cout << "C1:\n";
    std::cout << "  sizeof  = " << sizeof(C1) << '\n';
    std::cout << "  alignof = " << alignof(C1) << '\n';
    std::cout << "  offset c = " << offsetof(C1, c) << '\n';
    std::cout << "  offset d = " << offsetof(C1, d) << '\n';
    std::cout << "  offset i = " << offsetof(C1, i) << "\n\n";

    std::cout << "D1:\n";
    std::cout << "  sizeof  = " << sizeof(D1) << '\n';
    std::cout << "  alignof = " << alignof(D1) << '\n';
    std::cout << "  offset d = " << offsetof(D1, d) << '\n';
    std::cout << "  offset c = " << offsetof(D1, c) << '\n';
    std::cout << "  offset i = " << offsetof(D1, i) << "\n\n";

    std::cout << "OuterB:\n";
    std::cout << "  sizeof  = " << sizeof(OuterB) << '\n';
    std::cout << "  alignof = " << alignof(OuterB) << '\n';
    std::cout << "  offset x = " << offsetof(OuterB, x) << '\n';
    std::cout << "  offset a = " << offsetof(OuterB, a) << '\n';
    std::cout << "  offset y = " << offsetof(OuterB, y) << '\n';

    return 0;
}