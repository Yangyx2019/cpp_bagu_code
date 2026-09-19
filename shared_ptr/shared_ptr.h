#pragma once
#include <atomic>
#include <cstddef>
#include <utility>

// 面试简化版：仅管理单个 T 对象，不支持数组、自定义删除器、weak_ptr。
// 不同 SharedPtr 实例可以并发操作共享计数；同一个实例的并发修改、
// 以及所指对象的并发读写，仍需调用方同步。
template <typename T>
class SharedPtr {
public:
    SharedPtr() noexcept = default;

    explicit SharedPtr(T* ptr) : ptr_(ptr) {
        if (ptr_) {
            try {
                count_ = new std::atomic<std::size_t>(1);
            } catch (...) {
                delete ptr_;  // 分配计数失败时也不能泄漏对象
                throw;
            }
        }
    }

    SharedPtr(const SharedPtr& other) noexcept
        : ptr_(other.ptr_), count_(other.count_) {
        if (count_) ++(*count_);
    }

    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(other.ptr_), count_(other.count_) {
        other.ptr_ = nullptr;
        other.count_ = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        SharedPtr(other).swap(*this);  // 先取得新所有权，再释放旧所有权
        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        SharedPtr(std::move(other)).swap(*this);
        return *this;
    }

    ~SharedPtr() { release(); }

    T* get() const noexcept { return ptr_; }
    T& operator*() const noexcept { return *ptr_; }
    T* operator->() const noexcept { return ptr_; }
    explicit operator bool() const noexcept { return ptr_ != nullptr; }

    std::size_t use_count() const noexcept {
        return count_ ? count_->load() : 0;
    }

    void reset(T* ptr = nullptr) {
        // ptr 必须是尚未被智能指针管理的新对象，不能传入 get()。
        SharedPtr(ptr).swap(*this);
    }

    void swap(SharedPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(count_, other.count_);
    }

private:
    void release() noexcept {
        // 原子减一并判断结果，保证只有最后一个持有者执行 delete。
        // 不能写成先 --count，再单独 load 判断是否为零。
        if (count_ && --(*count_) == 0) {
            delete ptr_;
            delete count_;
        }
    }

    T* ptr_ = nullptr;
    std::atomic<std::size_t>* count_ = nullptr;
};