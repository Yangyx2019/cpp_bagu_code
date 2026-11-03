// select：每次传入关注集合，返回后逐个 FD_ISSET 检查谁可读。
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <sys/select.h>
#include <sys/wait.h>

int main() {
    int a[2], b[2];
    if (pipe(a) == -1 || pipe(b) == -1) { perror("pipe"); return 1; }
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }
    if (pid == 0) {
        close(a[0]); close(b[0]);
        if (write(a[1], "ABCDEF", 6) != 6 || write(b[1], "123456", 6) != 6) _exit(1);
        close(a[1]); close(b[1]);
        _exit(0);
    }
    close(a[1]); close(b[1]);
    int fds[2] = {a[0], b[0]};  // 两个管道模拟两个连接，省掉网络建连代码。
    if (fds[0] >= FD_SETSIZE || fds[1] >= FD_SETSIZE) {
        fprintf(stderr, "fd 编号超过 FD_SETSIZE，不能使用 FD_SET\n"); return 1;
    }

    int alive = 2;
    while (alive > 0) {
        fd_set reads;
        FD_ZERO(&reads);
        int maxfd = -1;
        // select 会把集合改为就绪结果，因此每轮都要重建关注集合。
        for (int fd : fds) {
            if (fd == -1) continue;
            FD_SET(fd, &reads);
            if (fd > maxfd) maxfd = fd;
        }
        int count = select(maxfd + 1, &reads, nullptr, nullptr, nullptr);
        // maxfd+1 是最大 fd 编号加一，不是连接数；nullptr 超时表示一直等。
        if (count == -1 && errno == EINTR) continue;
        if (count == -1) { perror("select"); return 1; }

        for (int& fd : fds) {  // 没有直接返回就绪列表，需要自己逐个检查。
            if (fd == -1 || !FD_ISSET(fd, &reads)) continue;
            char buf[2];
            ssize_t n = read(fd, buf, sizeof(buf));  // 故意每次只读 2 字节。
            if (n > 0) printf("fd=%d 读到: %.*s\n", fd, static_cast<int>(n), buf);
            // 没读完，下轮 select 仍会通知。EOF 也算可读。
            else if (n == 0) { close(fd); fd = -1; --alive; }
            else { perror("read"); return 1; }
        }
    }
    waitpid(pid, nullptr, 0);
}
