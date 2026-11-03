// epoll LT：先注册，之后每次只拿就绪列表。数据没读完，会继续通知。
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <sys/epoll.h>
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
    int epfd = epoll_create1(0);
    if (epfd == -1) { perror("epoll_create1"); return 1; }
    int fds[2] = {a[0], b[0]};
    for (int fd : fds) {
        epoll_event event{};
        event.events = EPOLLIN;  // 默认 LT，不需要写额外标志。
        event.data.fd = fd;      // 保存 fd，事件返回时能知道是谁就绪。
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) { perror("epoll_ctl"); return 1; }
    }  // 注册一次，内核记住关注集合，不用每轮把全部 fd 重新传进去。

    int alive = 2;
    while (alive > 0) {
        epoll_event events[2];
        int count = epoll_wait(epfd, events, 2, -1);
        if (count == -1 && errno == EINTR) continue;
        if (count == -1) { perror("epoll_wait"); return 1; }
        for (int i = 0; i < count; ++i) {  // 只遍历返回的就绪项。
            int fd = events[i].data.fd;
            char buf[2];
            ssize_t n = read(fd, buf, sizeof(buf));
            if (n > 0) printf("LT fd=%d 读到: %.*s\n", fd, static_cast<int>(n), buf);
            // 每次只读 2 字节也能读完：LT 会再次报告仍然可读的 fd。
            else if (n == 0) {
                epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                close(fd);
                --alive;
            } else { perror("read"); return 1; }
        }
    }
    close(epfd);
    waitpid(pid, nullptr, 0);
}
