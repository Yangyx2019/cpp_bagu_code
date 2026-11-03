#pragma once
#include <iostream>
template<typename T>
class Vector {
public:
    Vector() = default;

    Vector(const Vector& v) {
        // 申请空间
        reserve(v.capacity_);
        // 拷贝构造
        for (size_t i = 0; i < v.size_; ++i) {
            new (data_ + i) T(v[i]);
        }
        size_ = v.size_;
    }

    Vector& operator=(const Vector& v) {
        if (&v == this) return *this;
        clear();
        // 申请空间
        reserve(v.capacity_);
        // 拷贝构造
        for (size_t i = 0; i < v.size_; ++i) {
            new (data_ + i) T(v[i]);
        }
        size_ = v.size_;
        return *this;
    }

    Vector(Vector&& v) noexcept {
        size_ = v.size_;
        capacity_ = v.capacity_;
        data_ = v.data_;
        v.data_ = nullptr;
        v.size_ = 0;
        v.capacity_ = 0;
    }

    Vector& operator=(Vector&& v) noexcept {
        if (&v == this) return *this;
        clear();
        free(data_);
        size_ = v.size_;
        capacity_ = v.capacity_;
        data_ = v.data_;
        v.data_ = nullptr;
        v.size_ = 0;
        v.capacity_ = 0;
        return *this;
    }

    ~Vector() {
        // 旧空间析构
        clear();
        // 旧空间释放
        free(data_);
    }

    void push_back(const T& t) {
        if (size_ == capacity_) {
            size_t newCap = 2 * capacity_;
            if (newCap == 0) {
                newCap = 1;
            }
            reserve(newCap);
        }
        new (data_ + size_)T(t);
        ++size_;
    }

    void push_back(T&& t) {
        if (size_ == capacity_) {
            size_t newCap = 2 * capacity_;
            if (newCap == 0) {
                newCap = 1;
            }
            reserve(newCap);
        }
        new (data_ + size_)T(std::move(t));
        ++size_;
    }

    template<typename... Args>
    void emplace_back(Args&&... args) {
        if (size_ == capacity_) {
            size_t newCap = 2 * capacity_;
            if (newCap == 0) {
                newCap = 1;
            }
            reserve(newCap);
        }
        new (data_ + size_)T(std::forward<Args>(args)...);
        ++size_;
    }

    void pop_back() {
        if (size_ == 0) {
            return;
        }
        data_[size_ - 1].~T();
        --size_;
    }

    void shrink_to_fit() {
        if (size_ == capacity_) return;
        // 申请新的空间
        T* newData = static_cast<T*>(malloc(size_ * sizeof(T)));
        // 把旧的对象移动到新的空间上
        for (size_t i = 0; i < size_; ++i) {
            new (newData + i) T(std::move(data_[i]));
        }
        // 旧空间析构
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        // 旧空间释放
        free(data_);
        data_ = newData;
        capacity_ = size_;
    }

    /**
     * 扩容函数
     * 不改变size 改变capacity
     */
    void reserve(size_t capacity) {
        if (capacity <= capacity_) {
            return;
        }
        // 申请新的空间
        T* newData = static_cast<T*>(malloc(capacity * sizeof(T)));
        // 把旧的对象移动到新的空间上
        for (size_t i = 0; i < size_; ++i) {
            new (newData + i) T(std::move(data_[i]));
        }
        // 旧空间析构
        for (size_t i = 0; i < size_; ++i) {
            data_[i].~T();
        }
        // 旧空间释放
        free(data_);
        data_ = newData;
        capacity_ = capacity;
    }

    // 不改变capacity，size归零
    void clear() {
        for (size_t i = 0; i < size_; ++i) {
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
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};