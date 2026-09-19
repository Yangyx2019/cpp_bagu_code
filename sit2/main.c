/*
这里add是一个c++函数，使用了c++标准库
main.c是一个c程序，c程序想要用C++的函数，必须让C++的函数被extern C 包裹
这样才能顺利链接

实验步骤:
gcc -c main.c -o main.o
g++ -c add.cpp -o add.o
g++ main.o add.o 注意这里必须是g++
nm add.o会发现cpp文件里由于extern C的使用 add函数的符号还是C风格的
*/

int add(int, int);
int main()
{
    add(1, 2);
}