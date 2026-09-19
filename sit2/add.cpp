#include <iostream>
extern "C" {
int add(int a, int b) {
    std::cout << "result is:" << a + b << std::endl;
    return a + b; 
}
}