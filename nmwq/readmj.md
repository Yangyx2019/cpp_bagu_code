#### shared_ptr什么情况下会循环引用

```c++
#include <iostream>
#include <memory>

class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    ~A() { std::cout << "A destructor" << std::endl; }
};

class B {
public:
    std::shared_ptr<A> a_ptr;  // 使用 std::weak_ptr 打破循环引用
    ~B() { std::cout << "B destructor" << std::endl; }
};

int main() {
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();
    a->b_ptr = b;
    b->a_ptr = a;
    /**
      B和A堆上内存对应的引用计数是2
      当 a 析构会让a的引用计数-1 此时 A-> 1  B-> 2 
      但是 A的析构没有调用，因为A的计数不为0，所以这里没有机会析构A里的字段b_ptr 所以B的计数还是2
      当 b 析构会把b的引用计数 -1 同理 b的字段 a_ptr 没有机会析构 最后退出的时候 A->1 B->1
      这是循环引用导致的内存泄漏
    **/
    return 0;
}
```
更一般地，只要强引用形成闭环，就可能出现这种问题，包括自己持有自己，以及 A → B → C → A 这样的多对象环。通常让表示“回指”或“观察关系”的那条引用使用 weak_ptr。

#### 为什么weak_ptr可以解决循环引用

```c++
#include <iostream>
#include <memory>

class B;

class A {
public:
    std::shared_ptr<B> b_ptr;
    void hello() { std::cout << "Hello from A\n"; }
    ~A() { std::cout << "A destructor\n"; }
};

class B {
public:
    std::weak_ptr<A> a_ptr;

    void useA() {
        if (auto p = a_ptr.lock()) {
            // A 还活着，p 是 shared_ptr<A>
            p->hello();
        } else {
            // A 已被释放
            std::cout << "A no longer exists\n";
        }
    }

    ~B() { std::cout << "B destructor\n"; }
};

int main() {
    auto a = std::make_shared<A>();
    auto b = std::make_shared<B>();

    a->b_ptr = b;
    b->a_ptr = a;  // 可以直接从 shared_ptr 赋值，不增加强引用计数

    b->useA();    // 输出 Hello from A

    a.reset();    // A 的强引用计数变成 0，A 被释放
    b->useA();    // 输出 A no longer exists
}
```

weakptr的lock函数返回一个shared_ptr 如果资源已经释放会得到nullptr
因此也不需要担心lock了以后资源会被释放的问题

#### 智能指针可以管理一个数组吗？

可以，关键是保证用 delete[] 释放数组。unique_ptr<T[]> 从 C++11 起支持数组，shared_ptr<T[]> 从 C++17 起直接支持；更早的 shared_ptr 可以通过自定义删除器管理数组。另外，make_unique 的数组支持从 C++14 开始，make_shared 的数组支持从 C++20 开始。如果只是需要一个普通动态数组，我通常优先用 vector，因为它还管理长度和容量。

关键是std::unique_ptr<int[]> p(new int[10]{}); 这里delete的时候需要知道要 delete[]
不能delete  需要传递给智能指针 “int[]” 而不是 “int” 不然delete就是一个错误的delete

#### new 和 free可以混用吗？ 如果new了一个int free掉会怎么样？
首先：new的底层细节
auto p = new A();
这里new是一个操作符和 sizeof + - 一样，它的行为是：
1. 调用operator new(size_t size) 函数申请一片内存，size的大小取决于要new什么对象
通常由operator new(size_t size)内部由malloc实现内存申请

2. 在这片申请的内存上构造对象，但是不是手动调用构造函数，这是一个编译器行为，手动调用构造函数在语法上是不允许的。

3. 如果是  new(p) A(); 这里表示的是在p指针指向的空间上构造A对象，这里调用的是 operator new(size_t size, void* p)函数,这是一个operator new函数的重载，这个函数通常直接返回p，然后再在p上执行构造，这叫placement new。

所以不能保证operator new使用的是malloc申请的空间 因此使用free释放new出来的内存是一个未定义行为。

追问：为什么malloc一定要和free搭配使用呢？
答：这两个函数并不是系统调用，malloc底层使用系统调用申请内存，可能是（sbrk brk mmap）得到可用的堆内存空间，然后malloc和free自行维护了一套内存管理体系，所以malloc和free必须搭配使用，free一个不是malloc出来的内存可能会有问题。

*注意：free 也支持 calloc(分配内存并清零)、realloc(调整分配内存的大小) 等兼容分配接口返回的指针

#### 结构体内存对齐底层细节，为什么要这么设计，为什么这样可以解决这个问题？
#### 如果从32位系统换到64位系统结构体内存对齐会有什么区别？
编译器在创建结构体对象的时候保证了结构体的起始地址是 alignof 结构体的整数倍。
malloc出来的也满足，因为malloc出来的内存通常是有足够的对齐保证的。

#### 几种类型转换，父子类型之间的转换用什么？用static还是dynamic

分情况讨论：
1. 从子类到父类的转换
一般可以直接隐式转换或者static_cast 这两个没什么区别。 如果有菱形继承的子类转到有歧义的父类会编译错误，解决方式是可以使用虚继承。

2. 从父类到子类的转换

