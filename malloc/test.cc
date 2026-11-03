#include <cstdio>
#include <cstdlib>
#include <malloc.h>

int main() {
    void* p = std::malloc(1);
    if (p == nullptr) {
        return 1;
    }

    std::printf("requested: 1 byte\n");
    std::printf("usable: %zu bytes\n", malloc_usable_size(p));

    std::free(p);
}