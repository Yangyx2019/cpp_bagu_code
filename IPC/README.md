# IPC：一个文件学一种方式

Linux C++ 示例。每个 `.cpp` 都独立，全部步骤直接写在 `main()` 里，没有公共头文件、类或辅助函数。先看文件顶部的运行说明，再往下读。

## 怎么跑

```bash
cd /home/yangyx/Develop/cpp_bagu_code/IPC
g++ -std=c++17 01_pipe_parent_child.cpp -pthread -lrt -o demo
./demo
```

学其他例子时，只需替换编译命令中的源文件名。`-pthread -lrt` 供共享内存和信号量使用，其他例子带上也无妨。不需要 CMake。

| 文件 | 怎么运行 | 重点理解 |
| --- | --- | --- |
| `01_pipe_parent_child.cpp` | `./demo` | 先 pipe 再 fork；关闭不用的端；写端全部关闭后读到 EOF |
| `02_socketpair_parent_child.cpp` | `./demo` | 两个端点都能收发，父发 ping，子回 pong |
| `03_mmap_semaphore_parent_child.cpp` | `./demo` | MAP_SHARED 共享数据；sem_post/sem_wait 同步读写 |
| `04_fifo_unrelated.cpp` | A：`./demo read`；B：`./demo write` | 两个进程通过同一路径打开 FIFO |
| `05_sysv_queue_unrelated.cpp` | A：`./demo receive`；B：`./demo send` | 通过相同 key 找队列，按消息类型接收 |
| `06_shm_named_semaphore_unrelated.cpp` | 先 `./demo write`，退出后再 `./demo read` | 通过名字打开共享内存；读者读完后删除名字 |
| `07_unix_socket_unrelated.cpp` | A：`./demo receive`；B：`./demo send` | AF_UNIX 在本机通过路径通信 |
| `08_signal_unrelated.cpp` | A：`./demo wait`；B：`./demo send A打印的PID` | 信号适合通知“发生了一件事” |

A、B 表示两个终端，都进入本目录。先启动 A，看到等待提示再启动 B。例子固定使用短消息和资源名称，一次只运行一组。

## 什么场景选什么，为什么？

| 场景 | 方式 | 原因 |
| --- | --- | --- |
| 父子进程单向传数据 | 匿名管道 | fork 可以继承管道，接口简单 |
| 父子进程双向通信 | socketpair | 一对端点都能读写，比手工管理两条管道直接 |
| 独立进程单向传字节 | FIFO | 有路径，不需要继承父进程的 fd |
| 一条条发送任务/消息 | 消息队列 | 保留消息边界，System V 还可按类型接收 |
| 共享大量数据 | 共享内存＋同步机制 | 双方访问同一块内存，但必须协调何时读写 |
| 独立的本机程序交换消息 | Unix 域 socket | 支持数据报和流式通信，常用于本地服务 |
| 只需通知事件 | 信号 | 不需要为一个简单通知建立数据通道；普通信号可能合并 |

父子进程也能用 FIFO、消息队列等。区别是：匿名资源通常靠继承；独立进程通常靠名称、路径、key 或 PID 找到对方。

两个 socket 例子选用 `SOCK_DGRAM`，保留消息边界，便于先学 IPC；`SOCK_STREAM` 则是字节流，需要另外处理消息边界和短读写。普通变量在 fork 后不会共享修改；共享内存中的通知也不能用 `sleep` 或 `volatile` 替代。

## 中断后重跑

正常跑完会清理资源。若中途 Ctrl+C，先停止这一组进程，再清理本例留下的名称：

```bash
rm -f /tmp/cpp_ipc_demo.fifo /tmp/cpp_ipc_demo.sock
# 仅针对本例固定名称，Linux 中命名共享内存/信号量位于 /dev/shm：
rm -f /dev/shm/cpp_ipc_shm /dev/shm/sem.cpp_ipc_ready
# 消息队列：先把 05 编译成 demo，再执行：
./demo cleanup
```

这些是演示一条短消息的学习代码，没有加入超时、故障恢复或通用收发框架。先理解关键系统调用，再逐项扩展。
