#pragma once
template<typename T>
class unique_ptr {
public:
    explicit unique_ptr(T* ptr) : ptr_(ptr) {}
    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
    unique_ptr(unique_ptr&& uni_p) noexcept {
        ptr_ = uni_p.ptr_;
        uni_p.ptr_ = nullptr;
    }

    unique_ptr& operator=(unique_ptr&& uni_p) noexcept {
        if (this == &uni_p) {
            return *this;
        }
        destroy();
        ptr_ = uni_p.ptr_;
        uni_p.ptr_ = nullptr;
        return *this; // 任意忘记
    }

    ~unique_ptr() {
        destroy();
    }

    void reset(T* ptr) {
        if (ptr == ptr_) {
            return;
        }
        destroy();
        ptr_ = ptr;
    }

    T* operator->() const noexcept { return ptr_; } // 第一次写
    T& operator*() const noexcept { return *ptr_; } // 第一次写
    explicit operator bool() const noexcept { return ptr_ != nullptr; } // 第一次写
    T* get() const noexcept {
        return ptr_;
    }
private:
    void destroy() noexcept {
        delete ptr_;
        ptr_ = nullptr;
    }
    T* ptr_;
};