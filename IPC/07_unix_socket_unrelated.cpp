// 两个终端：./demo receive，然后 ./demo send。
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

int main(int argc, char* argv[]) {
    if (argc != 2 || (strcmp(argv[1], "receive") && strcmp(argv[1], "send"))) {
        printf("用法: ./demo receive|send\n"); return 1;
    }
    // AF_UNIX 是本机通信；这里选择数据报，一次收发一条完整的短消息。
    int fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (fd == -1) { perror("socket"); return 1; }
    sockaddr_un address{};
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, "/tmp/cpp_ipc_demo.sock");

    if (strcmp(argv[1], "receive") == 0) {
        // 接收端绑定路径，发送端通过这个路径找到它。
        if (bind(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
            perror("bind"); return 1;
        }
        puts("socket 已绑定，等待消息……");
        char buf[64];
        ssize_t n = recv(fd, buf, sizeof(buf), 0);
        if (n == -1) { perror("recv"); unlink(address.sun_path); return 1; }
        printf("收到: %.*s\n", static_cast<int>(n), buf);
        unlink(address.sun_path);
    } else {
        // 数据报不用 listen/accept/connect，直接 sendto 指定目标地址。
        if (sendto(fd, "hello socket", 12, 0, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == -1) {
            perror("sendto"); return 1;
        }
    }
    close(fd);
}
