#pragma once

#include <iostream>
#include <utility>
#include <new>

template<typename T>
class Vector {
public:
    Vector()
        : data_(nullptr), size_(0), capacity_(0)
    {}

    ~Vector()
    {
        clear();

        if (data_) {
            ::operator delete(data_);
        }
    }


    // 拷贝构造
    Vector(const Vector& other)
        : data_(nullptr), size_(0), capacity_(0)
    {
        reserve(other.capacity_);

        for (size_t i = 0; i < other.size_; i++) {
            new(data_ + i) T(other.data_[i]);
            size_++;
        }
    }


    // 移动构造
    Vector(Vector&& other) noexcept
        : data_(other.data_),
          size_(other.size_),
          capacity_(other.capacity_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }


    // 拷贝赋值
    Vector& operator=(const Vector& other)
    {
        if (this == &other)
            return *this;

        clear();

        reserve(other.capacity_);

        for (size_t i = 0; i < other.size_; i++) {
            new(data_ + i) T(other.data_[i]);
            size_++;
        }

        return *this;
    }


    // 移动赋值
    Vector& operator=(Vector&& other) noexcept
    {
        if (this == &other)
            return *this;

        clear();

        ::operator delete(data_);

        data_ = other.data_;
        size_ = other.size_;
        capacity_ = other.capacity_;

        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;

        return *this;
    }


public:

    void push_back(const T& value)
    {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        new(data_ + size_) T(value);
        size_++;
    }


    void push_back(T&& value)
    {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        new(data_ + size_) T(std::move(value));
        size_++;
    }


    template<typename... Args>
    void emplace_back(Args&&... args)
    {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : capacity_ * 2);
        }

        new(data_ + size_) T(std::forward<Args>(args)...);
        size_++;
    }



    void pop_back()
    {
        if (size_ > 0) {
            size_--;

            data_[size_].~T();
        }
    }


    void clear()
    {
        for (size_t i = 0; i < size_; i++) {
            data_[i].~T();
        }

        size_ = 0;
    }



    void reserve(size_t new_capacity)
    {
        if (new_capacity <= capacity_)
            return;


        // 申请裸内存
        T* new_data =
            static_cast<T*>(::operator new(sizeof(T) * new_capacity));


        // 移动旧对象
        for (size_t i = 0; i < size_; i++) {

            new(new_data + i)
                T(std::move(data_[i]));

            data_[i].~T();
        }


        // 释放旧空间
        ::operator delete(data_);


        data_ = new_data;
        capacity_ = new_capacity;
    }



    size_t size() const
    {
        return size_;
    }


    size_t capacity() const
    {
        return capacity_;
    }


    T& operator[](size_t index)
    {
        return data_[index];
    }


private:

    T* data_;          // 原始内存
    size_t size_;      // 已构造对象数量
    size_t capacity_;  // 总空间
};