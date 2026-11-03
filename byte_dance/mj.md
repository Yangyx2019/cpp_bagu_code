#### cmake和g++和gcc如何编debug包
-g
-DCMAKE_BUILD_TYPE=Debug
#### cmake 如何编译动态库的？
add_library(mymath SHARED add.cpp)
add_library(mymath STATIC add.cpp)

#### docker容器起了以后怎么想向外暴露端口怎么办

#### pybind11的使用

#### 如果希望c++的结构体里的内容紧挨着排列有没有办法？

可以通过 #pragma pack(push, 1) 等编译器扩展实现紧凑布局，再用 pop 恢复设置。代价是成员可能未对齐，访问性能和指针使用都需要注意。

#### c++里动态库有两种打开方式  
一种是运行启动的时候就会找动态库加入内存（不一定，如果已经在内存里就不会加入）
一种是跑到具体函数的时候再去找库加入内存（不一定，如果已经在内存里就不会加入）

函数	作用
dlopen()	打开动态库，获取句柄
dlsym()	根据符号名获取函数或变量地址
dlclose()	释放动态库句柄，不一定立即卸载
dlerror()	获取错误信息
```c++
#include <dlfcn.h>
#include <iostream>

int main() {
    // 1. 加载动态库
    void* handle = dlopen("./libplugin.so", RTLD_NOW);
    if (!handle) {
        std::cerr << dlerror() << '\n';
        return 1;
    }

    // 2. 查找函数
    dlerror(); // 清除之前的错误
    auto add = reinterpret_cast<int (*)(int, int)>(
        dlsym(handle, "add")
    );

    if (const char* error = dlerror()) {
        std::cerr << error << '\n';
        dlclose(handle);
        return 1;
    }

    // 3. 调用函数
    std::cout << add(2, 3) << '\n'; // 输出 5

    // 4. 释放句柄，之后不再使用该函数指针
    dlclose(handle);
}
```


#### moe了解吗？


#### linux命令
如何输出所有python进程的进程号？
pgrep python
pgrep -a python


