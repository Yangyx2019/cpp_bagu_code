#ifndef SCREENSHOT_SHARED_PTR_H
#define SCREENSHOT_SHARED_PTR_H

#include <atomic>
#include <cstddef>

// 根据截图整理，保留原实现的问题，不应用修复。
// 截图中被遮挡的 release 条件按 fetch_sub(...) == 1 补全；
// 移动构造初始化列表等遮挡部分按可见上下文补全。
// 截图中的 i:、m: 是编辑器参数提示，不是 C++ 源码。
// 适用范围：单个 T 对象，不支持数组、自定义删除器、配置器或弱引用。
// 线程安全边界：仅共享引用计数的增减是原子的。不同实例可以并发操作；
// 同一个实例的并发修改，以及被管理对象的并发读写，仍需外部同步。

template <typename T>
class shared_ptr {
private:
    T* ptr;                                    // 指向管理的对象
    std::atomic<std::size_t>* ref_count;       // 原子引用计数

    void release() {
        // 此处条件右半部分在截图中被遮挡，下面是按上下文补全的版本。
        // fetch_sub 返回减之前的值，旧值为 1 才表示当前线程是最后一个持有者。
        if (ref_count &&
            ref_count->fetch_sub(1, std::memory_order_acq_rel) == 1) {
            delete ptr;
            delete ref_count;
        }
        // 注意：这里不会清空成员。析构时无妨，但调用者后续必须正确更新成员；
        // 下方 reset 分配失败时没有完成更新，会因此留下失效的计数指针。
    }

public:
    shared_ptr() : ptr(nullptr), ref_count(nullptr) {}

    // 问题 1：分配 ref_count 失败时，构造失败，不会调用本类析构函数，
    // 传入的 p 因而没有被释放。
    // 改法：先把 ref_count 初始化为 nullptr，再在函数体中 try 分配；
    // catch (...) { delete p; throw; }。
    explicit shared_ptr(T* p)
        : ptr(p),
          ref_count(p ? new std::atomic<std::size_t>(1) : nullptr) {}

    ~shared_ptr() {
        release();
    }

    shared_ptr(const shared_ptr<T>& other)
        : ptr(other.ptr), ref_count(other.ref_count) {
        if (ref_count) {
            ref_count->fetch_add(1, std::memory_order_relaxed);
        }
    }

    // 问题 2：先 release 再读取 other，在 other 是被管理对象的成员时，
    // release 可能销毁 other，随后访问 other 就是访问已销毁对象。
    // 例：struct Node { shared_ptr<Node> next; };
    //     shared_ptr<Node> p(new Node);
    //     p->next.reset(new Node);
    //     p = p->next;
    // this != &other 只能排除直接自赋值，不能排除上面的情况。
    // 改法：添加交换两个成员的 noexcept swap，然后写：
    // shared_ptr(other).swap(*this); return *this;
    // 临时对象先取得新所有权，交换后再释放旧所有权。
    shared_ptr<T>& operator=(const shared_ptr<T>& other) {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            if (ref_count) {
                ref_count->fetch_add(1, std::memory_order_relaxed);
            }
        }
        return *this;
    }

    // 截图中的初始化列表右侧被遮挡，按上下文补全。
    // 注释纠正：std::swap 并非一律要求移动操作 noexcept；
    // noexcept 会影响其异常规格，也会影响某些容器选择移动还是拷贝。
    shared_ptr(shared_ptr<T>&& other) noexcept
        : ptr(other.ptr), ref_count(other.ref_count) {
        other.ptr = nullptr;
        other.ref_count = nullptr;
    }

    // 问题 3：与拷贝赋值相同，先 release 可能销毁作为来源的成员。
    // 例：p = std::move(p->next);
    // 改法：shared_ptr(std::move(other)).swap(*this); return *this;
    // 使用 std::move/std::swap 时需要包含 <utility>。
    shared_ptr<T>& operator=(shared_ptr<T>&& other) noexcept {
        if (this != &other) {
            release();
            ptr = other.ptr;
            ref_count = other.ref_count;
            other.ptr = nullptr;
            other.ref_count = nullptr;
        }
        return *this;
    }

    // 截图原注释“const 表示该函数会修改对象的状态”写反了。
    // const 限制智能指针本身普通成员的修改，不限制被指向的 T 对象。
    // 返回 T& 是正确的；调用前须确保 ptr 非空。
    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    std::size_t use_count() const {
        // acquire 不算错误；只观察计数时 relaxed 即可。
        // 返回值只是并发瞬间的计数，不能据此保证后续独占访问对象。
        return ref_count ? ref_count->load(std::memory_order_acquire) : 0;
    }

    T* get() const {
        return ptr;
    }

    // 问题 4：最后一行 new 抛出异常时，旧所有权已释放，ptr 已换成 p，
    // 但赋值尚未完成，ref_count 仍是旧地址：可能悬空，或属于其他对象。
    // 后续析构可能访问已释放内存或错误修改其他对象的引用计数；
    // 新传入的 p 也没有得到正确管理。
    // 改法：先修复上面的裸指针构造，再写 shared_ptr(p).swap(*this);
    // 这样新计数分配失败时，本实例原来的所有权保持不变。
    // 使用限制：不要 reset(get())，也不要传入已被其他智能指针管理的指针；
    // 这会产生悬空指针或重复管理，标准 shared_ptr 同样不能这样使用。
    void reset(T* p = nullptr) {
        release();
        ptr = p;
        ref_count = p ? new std::atomic<std::size_t>(1) : nullptr;
    }
};

#endif  // SCREENSHOT_SHARED_PTR_H