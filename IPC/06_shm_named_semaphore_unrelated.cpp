// 先 ./demo write，再 ./demo read。写者退出后，命名资源仍然存在。
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>

int main(int argc, char* argv[]) {
    if (argc != 2 || (strcmp(argv[1], "write") && strcmp(argv[1], "read"))) {
        printf("用法: ./demo write|read\n"); return 1;
    }
    bool writer = strcmp(argv[1], "write") == 0;
    // 共享内存有名字，因此不需要通过 fork 继承。
    int fd = shm_open("/cpp_ipc_shm", writer ? O_CREAT | O_EXCL | O_RDWR : O_RDWR, 0600);
    if (fd == -1) { perror("shm_open"); return 1; }
    if (writer && ftruncate(fd, 64) == -1) { perror("ftruncate"); shm_unlink("/cpp_ipc_shm"); return 1; }
    auto* text = static_cast<char*>(mmap(nullptr, 64, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
    if (text == MAP_FAILED) { perror("mmap"); if (writer) shm_unlink("/cpp_ipc_shm"); return 1; }
    // 命名信号量负责通知；它不负责存放正文。
    sem_t* ready = sem_open("/cpp_ipc_ready", writer ? O_CREAT | O_EXCL : 0, 0600, 0);
    if (ready == SEM_FAILED) { perror("sem_open"); if (writer) shm_unlink("/cpp_ipc_shm"); return 1; }

    if (writer) {
        strcpy(text, "hello shared memory");
        if (sem_post(ready) == -1) { perror("sem_post"); return 1; }
        puts("数据已写好，现在运行 ./demo read");
    } else {
        int result;
        do { result = sem_wait(ready); } while (result == -1 && errno == EINTR);
        if (result == -1) { perror("sem_wait"); return 1; }
        printf("读到: %s\n", text);  // 等写者通知后再读，而不是用 sleep 猜测。
        shm_unlink("/cpp_ipc_shm");  // 本例约定：读完的人清理名字。
        sem_unlink("/cpp_ipc_ready");
    }
    sem_close(ready);
    munmap(text, 64);
    close(fd);
}
