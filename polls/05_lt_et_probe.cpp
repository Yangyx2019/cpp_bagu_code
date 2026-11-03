// 看现象：写 6 字节，只读 2 字节，再次等待，会不会继续通知？
#include <cstdio>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

int main() {
    for (int edge = 0; edge <= 1; ++edge) {  // 分别运行 LT、ET；没有其他线程或进程。
        int fd[2];
        if (pipe(fd) == -1) { perror("pipe"); return 1; }
        int flags = fcntl(fd[0], F_GETFL);
        if (flags == -1 || fcntl(fd[0], F_SETFL, flags | O_NONBLOCK) == -1) { perror("fcntl"); return 1; }
        int epfd = epoll_create1(0);
        if (epfd == -1) { perror("epoll_create1"); return 1; }
        epoll_event event{};
        event.events = EPOLLIN;
        if (edge) event.events |= EPOLLET;
        event.data.fd = fd[0];
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd[0], &event) == -1) { perror("epoll_ctl"); return 1; }

        if (write(fd[1], "abcdef", 6) != 6) { perror("write"); return 1; }
        // 故意保留写端：排除“关闭写端也会产生事件”的干扰。
        if (epoll_wait(epfd, &event, 1, 1000) != 1) { fprintf(stderr, "没收到首次事件\n"); return 1; }
        char buf[8];
        if (read(fd[0], buf, 2) != 2) { perror("read"); return 1; }
        int count = epoll_wait(epfd, &event, 1, 0);  // 超时 0：立即查看，不阻塞。
        if (count == -1) { perror("epoll_wait"); return 1; }
        printf("%s：剩余 4 字节没读，再次 wait 返回 %d 个事件\n", edge ? "ET" : "LT", count);
        if (count != (edge ? 0 : 1)) return 1;

        // ET 没通知不代表没数据！直接 read 仍可以读到剩余 4 字节。
        if (read(fd[0], buf, sizeof(buf)) != 4) { fprintf(stderr, "剩余数据错误\n"); return 1; }
        ssize_t n = read(fd[0], buf, sizeof(buf));
        if (n != -1 || errno != EAGAIN) { fprintf(stderr, "预期 EAGAIN\n"); return 1; }
        puts("    继续读完剩余 4 字节，再 read 才得到 EAGAIN\n");
        close(fd[0]); close(fd[1]); close(epfd);
    }
}
