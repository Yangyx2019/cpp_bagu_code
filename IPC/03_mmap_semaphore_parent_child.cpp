// 共享内存传数据，信号量通知“数据写好了”。普通变量在 fork 后不共享修改。
#include <cstdio>
#include <cerrno>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>

int main() {
    struct Shared { sem_t ready; int number; };  // 只是共享数据布局，没有封装。
    auto* data = static_cast<Shared*>(mmap(nullptr, sizeof(Shared), PROT_READ | PROT_WRITE,
                                          MAP_SHARED | MAP_ANONYMOUS, -1, 0));
    if (data == MAP_FAILED) { perror("mmap"); return 1; }
    // pshared=1 表示跨进程；初始值 0 表示子进程先等通知。
    if (sem_init(&data->ready, 1, 0) == -1) { perror("sem_init"); return 1; }
    pid_t pid = fork();
    if (pid == -1) { perror("fork"); return 1; }

    if (pid == 0) {
        int result;
        do { result = sem_wait(&data->ready); } while (result == -1 && errno == EINTR);
        if (result == -1) { perror("sem_wait"); return 1; }
        printf("子进程读到: %d\n", data->number);
        munmap(data, sizeof(Shared));
        return 0;
    }
    data->number = 42;
    if (sem_post(&data->ready) == -1) { perror("sem_post"); return 1; }  // 写完再通知。
    waitpid(pid, nullptr, 0);
    sem_destroy(&data->ready);  // 确认子进程不再使用后才能销毁。
    munmap(data, sizeof(Shared));
}
