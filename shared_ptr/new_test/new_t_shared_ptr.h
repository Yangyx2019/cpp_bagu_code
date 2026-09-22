#pragma once
#include <atomic>
template<typename T>
class shared_ptr {
public:
    explicit shared_ptr(T* ptr) : ptr_(ptr) {
        if (ptr_) {
            cnt_ = new std::atomic<int>(1);
        }
    }

    shared_ptr(const shared_ptr& sp) : ptr_(sp.ptr_), cnt_(sp.cnt_) {
        if (ptr_) {
            ++*cnt_;
        }
    }

    shared_ptr(shared_ptr&& sp) noexcept : ptr_(sp.ptr_), cnt_(sp.cnt_) {
        sp.ptr_ = nullptr;
        sp.cnt_ = nullptr;
    }

    shared_ptr& operator=(const shared_ptr& sp) noexcept {
        if (this == &sp) {
            return *this;
        }
        release();
        ptr_ = sp.ptr_;
        cnt_ = sp.cnt_;
        if (cnt_) ++*cnt_;
        return *this;
    }

    shared_ptr& operator=(shared_ptr&& sp) noexcept {
        if (this == &sp) {
            return *this;
        }
        release();
        ptr_ = sp.ptr_;
        cnt_ = sp.cnt_;
        sp.ptr_ = nullptr;
        sp.cnt_ = nullptr;
        return *this;
    }

    ~shared_ptr() {
        release();
    }

    void reset(T* ptr) {
        if (ptr == ptr_) {
            return;
        }
        release();
        ptr_ = ptr;
        if (ptr_) {
            cnt_ = new std::atomic<int>(1);
        }
    }

private:
    void release() {
        if (ptr_) {
            // 注意，这里的--*cnt是个原子操作，如果不是是会有问题的
            if (--*cnt_ == 0) {
                delete ptr_;
                delete cnt_;
            }
            ptr_ = nullptr;
            cnt_ = nullptr;
        }
    }
    T* ptr_;
    std::atomic<int>* cnt_ = nullptr;
};