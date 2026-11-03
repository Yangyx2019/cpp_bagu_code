// socketpair：一次得到两个相连的端点，父子进程可以双向通信。
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    // 用 SOCK_DGRAM 保留消息边界；每次 send 对应一条消息，避免引入流式协议。
    if (socketpair(AF_UNIX, SOCK_DGRAM, 0, fd) == -1) { perror("socketpair"); return 1; }
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }
    char buf[64];

    if (pid == 0) {
        close(fd[0]);
        ssize_t n = recv(fd[1], buf, sizeof(buf), 0);
        if (n == -1) { perror("recv"); return 1; }
        printf("子进程收到: %.*s\n", static_cast<int>(n), buf);
        if (send(fd[1], "pong", 4, 0) == -1) { perror("send"); return 1; }
        close(fd[1]);
    } else {
        close(fd[1]);
        if (send(fd[0], "ping", 4, 0) == -1) { perror("send"); return 1; }
        ssize_t n = recv(fd[0], buf, sizeof(buf), 0);
        if (n == -1) { perror("recv"); return 1; }
        printf("父进程收到: %.*s\n", static_cast<int>(n), buf);
        close(fd[0]);
        waitpid(pid, nullptr, 0);
    }
}
