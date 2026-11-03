// poll：传入 pollfd 数组，返回后检查每项的 revents。
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <poll.h>
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
    // fd 是编号；events 是“我关心什么”；revents 是“实际发生什么”。
    pollfd fds[2] = {{a[0], POLLIN, 0}, {b[0], POLLIN, 0}};

    int alive = 2;
    while (alive > 0) {
        int count = poll(fds, 2, -1);  // 2 是数组元素个数；-1 表示一直等。
        if (count == -1 && errno == EINTR) continue;
        if (count == -1) { perror("poll"); return 1; }
        // events 不会被覆盖，无须像 select 那样重建集合，但仍需遍历数组。
        for (auto& item : fds) {
            if (item.fd == -1) continue;
            if (item.revents & (POLLERR | POLLNVAL)) { fprintf(stderr, "poll fd 错误\n"); return 1; }
            // 写端关闭会产生 HUP，但可能还有数据没读完，不能立刻 close。
            if (!(item.revents & (POLLIN | POLLHUP))) continue;
            char buf[2];
            ssize_t n = read(item.fd, buf, sizeof(buf));
            if (n > 0) printf("fd=%d 读到: %.*s\n", item.fd, static_cast<int>(n), buf);
            else if (n == 0) {
                close(item.fd);
                item.fd = -1;  // poll 会忽略 fd 为负数的项。
                --alive;
            } else { perror("read"); return 1; }
        }
    }
    waitpid(pid, nullptr, 0);
}
