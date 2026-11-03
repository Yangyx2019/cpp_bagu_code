// 两个终端：./demo read，然后 ./demo write。
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char* argv[]) {
    if (argc != 2 || (strcmp(argv[1], "read") && strcmp(argv[1], "write"))) {
        printf("用法: ./demo read|write\n"); return 1;
    }
    const char* path = "/tmp/cpp_ipc_demo.fifo";  // 无亲缘关系的进程通过路径找到同一管道。
    if (strcmp(argv[1], "read") == 0) {
        if (mkfifo(path, 0600) == -1) { perror("mkfifo"); return 1; }
        puts("FIFO 已创建，等待写者……");
        int fd = open(path, O_RDONLY);  // 默认阻塞，直到另一端打开写端。
        if (fd == -1) { perror("open"); unlink(path); return 1; }
        char buf[64];
        ssize_t n;
        while ((n = read(fd, buf, sizeof(buf))) > 0)
            printf("读者收到: %.*s\n", static_cast<int>(n), buf);
        close(fd);
        unlink(path);  // 读者负责删除自己创建的 FIFO。
        if (n == -1) { perror("read"); return 1; }
    } else {
        int fd = open(path, O_WRONLY);
        if (fd == -1) { perror("open，请先启动 read"); return 1; }
        if (write(fd, "hello FIFO", 10) != 10) { perror("write"); return 1; }
        close(fd);  // 读者读完后得到 EOF。
    }
}
