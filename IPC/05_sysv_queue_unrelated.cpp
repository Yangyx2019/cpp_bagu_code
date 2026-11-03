// 两个终端：./demo receive，然后 ./demo send。
#include <cstdio>
#include <cstring>
#include <sys/msg.h>

int main(int argc, char* argv[]) {
    if (argc != 2 || (strcmp(argv[1], "receive") && strcmp(argv[1], "send") && strcmp(argv[1], "cleanup"))) {
        printf("用法: ./demo receive|send|cleanup\n"); return 1;
    }
    bool receiver = strcmp(argv[1], "receive") == 0;
    // 两个独立进程约定相同 key；独占创建，避免接管已经存在的队列。
    int id = msgget(0x20261013, receiver ? IPC_CREAT | IPC_EXCL | 0600 : 0600);
    if (id == -1) { perror("msgget"); return 1; }
    if (strcmp(argv[1], "cleanup") == 0) {
        if (msgctl(id, IPC_RMID, nullptr) == -1) { perror("msgctl"); return 1; }
        return 0;
    }
    // System V 规定第一个字段必须是 long 类型的消息类型，且发送时必须 > 0。
    struct Message { long type; char text[64]; } message{1, "hello queue"};
    if (receiver) {
        puts("队列已创建，等待类型为 1 的消息……");
        ssize_t n = msgrcv(id, &message, sizeof(message.text), 1, 0);
        if (n == -1) { perror("msgrcv"); msgctl(id, IPC_RMID, nullptr); return 1; }
        printf("收到: %.*s\n", static_cast<int>(n), message.text);
        msgctl(id, IPC_RMID, nullptr);  // 队列不会随进程退出自动消失，要显式删除。
    } else {
        // 长度只算正文，不包含开头的 long type。队列保留每条消息的边界。
        if (msgsnd(id, &message, strlen(message.text), 0) == -1) { perror("msgsnd"); return 1; }
    }
}
