// 终端 A：./demo wait。终端 B：./demo send A打印的PID。
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <climits>
#include <signal.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    if (argc == 2 && strcmp(argv[1], "wait") == 0) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        // 先阻塞，再公布 PID：信号提前到达会保持 pending，不会错过通知。
        if (sigprocmask(SIG_BLOCK, &set, nullptr) == -1) { perror("sigprocmask"); return 1; }
        printf("接收进程 PID=%d\n", getpid());
        fflush(stdout);
        int number;
        int error = sigwait(&set, &number);  // 同步等待，无需写信号处理函数。
        if (error != 0) { fprintf(stderr, "sigwait: %s\n", strerror(error)); return 1; }
        printf("收到 SIGUSR1，可以开始干活了\n");
    } else if (argc == 3 && strcmp(argv[1], "send") == 0) {
        char* end;
        errno = 0;
        long pid = strtol(argv[2], &end, 10);
        // 必须拒绝 0、负数，它们在 kill 中有“向进程组发送”的特殊含义。
        if (errno || end == argv[2] || *end || pid <= 0 || pid > INT_MAX) {
            fprintf(stderr, "PID 必须是有效正整数\n"); return 1;
        }
        if (kill(static_cast<pid_t>(pid), SIGUSR1) == -1) { perror("kill"); return 1; }
    } else {
        printf("用法: ./demo wait | ./demo send PID\n"); return 1;
    }
}
