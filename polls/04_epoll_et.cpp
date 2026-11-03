// epoll ET：与 LT 对照，重点只有 EPOLLET、O_NONBLOCK、循环读到 EAGAIN。
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
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
        int flags = fcntl(fd, F_GETFL);
        if (flags == -1 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) { perror("fcntl"); return 1; }
        epoll_event event{};
        event.events = EPOLLIN | EPOLLET;  // 比 LT 多这个 EPOLLET。
        event.data.fd = fd;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1) { perror("epoll_ctl"); return 1; }
    }

    int alive = 2;
    while (alive > 0) {
        epoll_event events[2];
        int count = epoll_wait(epfd, events, 2, -1);
        if (count == -1 && errno == EINTR) continue;
        if (count == -1) { perror("epoll_wait"); return 1; }
        for (int i = 0; i < count; ++i) {
            int fd = events[i].data.fd;
            while (true) {  // 关键！不能像 LT 一样读一次就回去等通知。
                char buf[2];
                ssize_t n = read(fd, buf, sizeof(buf));
                if (n > 0) printf("ET fd=%d 读到: %.*s\n", fd, static_cast<int>(n), buf);
                else if (n == 0) {  // 对端已关闭且数据读完。
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                    close(fd);
                    --alive;
                    break;
                } else if (errno == EINTR) continue;
                else if (errno == EAGAIN || errno == EWOULDBLOCK) break;  // 暂时读空，才能回去等。
                else { perror("read"); return 1; }
            }
        }
    }
    close(epfd);
    waitpid(pid, nullptr, 0);
}
