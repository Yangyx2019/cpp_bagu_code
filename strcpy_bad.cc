#include <iostream>
#include <cstring>
using namespace std;
// 需要使用g++ -O0 -fno-stack-protector main.cc编译，否则可能看不到实验现象就abort了。 
// -O0表示禁用优化， -fno-stack-protector 表示不使用栈越界保护
/*
strcpy没有缓冲区溢出判断，下面的例子中向ar写入过量数据会覆盖x的内容。
*/
int main()
{
    uint32_t x = 0x12345678;
    char ar[10] = {0};
    cout << "x的地址是:" << hex << &x << endl;
    cout << "ar的地址是:" << hex << &ar << endl;
    /*
     * 思考：cout使用什么方法输出16进制整数？
     */
    cout << hex << x << endl;
    strcpy(ar, "12345678910");
    cout << hex << x << endl;
}