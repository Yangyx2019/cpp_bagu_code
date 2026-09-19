#include <iostream>
/*
实验目的：测试extern "C" 在C++调用C函数时的作用
步骤：
1. g++ -c main.cc -o main.o
2. gcc -c add.c -o add.o
3. nm add.o     nm main.o
在第三步会发现add.o和main.o中都有符号 "add"，此时 g++ add.o main.o 会成功

如果删除13行的extern "C" 那么main.o中的add符号变成C++风格的：
此时链接失败，因为名字不匹配
*/
extern "C" {
int add(int, int);
}

int main()
{
    std::cout << add(1, 2) << std::endl;
}