#pragma once
#include <iostream>
#include <utility>
#include <new>

template<typename T>
class vector {
public:
    vector() = default;
    ~vector() {
        for (int i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        free(data_);
    }

    vector(const vector<T>& v) {
        reserve(v.capacity_);
        // 空间够了，按size调用拷贝构造
        for (int i = 0; i < v.size_; ++i) {
            new(data_ + i) T(v[i]);
            ++size_;
        }
    }

    vector(vector<T>&& v) noexcept {
        size_ = v.size_;
        data_ = v.data_;
        capacity_ = v.capacity_;
        v.capacity_ = 0;
        v.size_ = 0;
        v.data_ = nullptr;
    }

    vector& operator=(const vector<T>& v) {
        if (this == &v) return *this;
        // 释放自己的资源，拷贝v的资源
        clear();
        reserve(v.capacity_);
        for (int i = 0; i < v.size_; ++i) {
            new(data_ + i) T(v.data_[i]);
            ++size_;
        }
        return *this;
    }

    vector& operator=(vector<T>&& v) noexcept {
        if (this == &v) return *this;
        // 释放自己的资源，移动v的资源
        clear();
        free(data_);
        data_ = v.data_;
        size_ = v.size_;
        capacity_ = v.capacity_;
        v.data_ = nullptr;
        v.capacity_ = 0;
        v.size_ = 0;
        return *this;
    }

    /*
        reserve的行为是扩容：
        如果参数比当前的容量小或等，什么都不做返回
        否则：
        1. 申请一片新空间，大小是capacity
        2. 把在新空间上移动构造旧空间的所有T
        3. 在旧空间上析构T
        4. free旧空间
    */
    void reserve(size_t capacity) {
        if (capacity_ >= capacity) {
            return;
        }

        auto data = static_cast<T*>(malloc(sizeof(T) * capacity));
        for (int i = 0; i < size_; ++i) {
            new(data + i) T(std::move(data_[i]));
            data_[i].~T();
        }

        free(data_);
        data_ = data;
        capacity_ = capacity;
    }

    void push_back(const T& v) {
        if (size_ == capacity_) {
            // 扩容
            size_t new_c = 1;
            if (capacity_ != 0) {
                new_c = capacity_ * 2;
            }
            reserve(new_c);
        }
        new(data_ + size_) T(v);
        ++size_;
    }

    void push_back(T&& v) {
        if (size_ == capacity_) {
            // 扩容
            size_t new_c = 1;
            if (capacity_ != 0) {
                new_c = capacity_ * 2;
            }
            reserve(new_c);
        }
        new(data_ + size_) T(std::move(v));
        ++size_;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : 2 * capacity_);
        }
        new (data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() {
        if (size_ == 0) {
            return;
        }
        data_[size_ - 1].~T();
        --size_;
    }

    void clear() {
        for (int i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        size_ = 0;
    }

    T& operator[](size_t idx) {
        return data_[idx];
    }

    const T& operator[](size_t idx) const {
        return data_[idx];
    }

private:
    size_t capacity_ = 0;
    size_t size_ = 0;
    T* data_ = nullptr;
};