#include <atomic>
template<typename T>
class shared_ptr {
public:
    explicit shared_ptr() {}
    explicit shared_ptr(T* p) {
        ptr_ = p;
        try {
            if(p) {
                cnt_ = new std::atomic<size_t>{1};
            }
        } catch(const std::bad_alloc&) {
            delete p;
            ptr_ = nullptr;
        }
    }

    ~shared_ptr() {
        release();
    }

    // 拷贝构造函数
    shared_ptr(const shared_ptr<T>& sp_1) {
        this->ptr_ = sp_1.ptr_;
        this->cnt_ = sp_1.cnt_;
        if (this->cnt_) {
            ++(*cnt_); // 这里(*cnt_)++ 有区别吗？
        }
    }

    // 移动构造函数
    shared_ptr(shared_ptr<T>&& sp_1) noexcept {
        /**
        shared_ptr<A> p1(new A);
        shared_ptr<A> p2 = std::move(p1); or  shared_ptr<A> p2(std::move(p1));
        */
        this->ptr_ = sp_1.ptr_;
        this->cnt_ = sp_1.cnt_;
        sp_1.ptr_ = nullptr;
        sp_1.cnt_ = nullptr;
    }

    // 拷贝赋值函数
    shared_ptr<T>& operator=(const shared_ptr<T>& sp_1) noexcept {
        if(this == &sp_1) {
            return *this;
        }
        release();
        ptr_ = sp_1.ptr_;
        cnt_ = sp_1.cnt_;
        if(cnt_) {
            ++*cnt_;
        }
        return *this;
    }

    // 移动赋值函数
    shared_ptr<T>& operator=(shared_ptr<T>&& sp_1) noexcept {
        if(this == &sp_1) {
            return *this;
        }
        release();
        ptr_ = sp_1.ptr_;
        cnt_ = sp_1.cnt_;
        sp_1.ptr_ = nullptr;
        sp_1.cnt_ = nullptr;
        return *this;
    }
    size_t getCnt() const {
        if(cnt_) {
            return *cnt_;
        }
        return 0;
    }

    T* getPtr() const {
        return ptr_;
    }

    void reset(T* p = nullptr) {
        if (this == p) return;
        release();
        ptr_ = p;
        try {
            if(p) {
                cnt_ = new std::atomic<size_t>{1};
            }
        } catch(const std::bad_alloc&) {
            delete p;
            ptr_ = nullptr;
        }
    }
private:
    void release() { // 当前shared_ptr不打算管目前的ptr_时使用
        if (cnt_ && --*cnt_ == 0) {
            delete ptr_;
            delete cnt_;
        }
        ptr_ = nullptr;
        cnt_ = nullptr;
    }

    T* ptr_ = nullptr;
    std::atomic<size_t>* cnt_ = nullptr;
};