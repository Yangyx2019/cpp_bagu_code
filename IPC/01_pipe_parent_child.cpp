// 匿名管道：父进程写，子进程读。先 pipe，再 fork，子进程才能继承管道。
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) { perror("pipe"); return 1; }
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    if (pid == 0) {
        close(fd[1]);  // 子进程不用写端，必须关闭，否则自己会阻止 EOF 出现。
        char buf[64];
        ssize_t n;
        while ((n = read(fd[0], buf, sizeof(buf))) > 0)
            printf("子进程收到: %.*s\n", static_cast<int>(n), buf);
        // read 返回 0：所有写端都关闭了。返回 -1：发生错误。
        if (n == -1) { perror("read"); return 1; }
        close(fd[0]);
    } else {
        close(fd[0]);
        if (write(fd[1], "hello", 5) != 5) { perror("write"); return 1; }
        close(fd[1]);  // 先关闭写端，子进程才能结束 read 循环。
        waitpid(pid, nullptr, 0);  // 回收子进程。
    }
}
