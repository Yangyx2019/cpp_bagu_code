#pragma once

class Singleton {
public:
    static Singleton& getInstance() {
        return example_;
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    int val_ = 0;
    private:
    Singleton(){};
    Singleton(int val) : val_(val) {};
    static Singleton example_;
}

Singleton::example_{2};

