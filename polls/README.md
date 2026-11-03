# select / poll / epoll：只看核心

每个 `.cpp` 独立，代码全在 `main()`，没有公共头文件、类或辅助函数。

**统一场景：子进程向两个管道写数据，父进程同时等它们可读。** 用管道省掉 TCP 建连和回显缓冲，让你直接看到三种等待方式。实际 socket 也可以放进这些关注集合。

## 先运行，看到差别

```bash
cd /home/yangyx/Develop/cpp_bagu_code/polls
g++ -std=c++17 05_lt_et_probe.cpp -o demo
./demo
```

关键输出：

```text
LT：剩余 4 字节没读，再次 wait 返回 1 个事件
ET：剩余 4 字节没读，再次 wait 返回 0 个事件
```

**LT：没读完，继续通知。ET：不能指望剩余数据再次触发通知，所以要循环读到 EAGAIN。** `EAGAIN` 是“现在没数据了”，`read=0` 是“写端关闭且数据已读完”，不要混淆。

## 然后依次读这几个文件

| 文件 | 只抓住这几个地方 |
| --- | --- |
| `01_select.cpp` | `FD_SET` 加关注 → `select` 等待 → `FD_ISSET` 逐个检查；每轮重建集合 |
| `02_poll.cpp` | `pollfd` 数组 → `poll` 等待 → 遍历 `revents`；关注的 `events` 不会被覆盖 |
| `03_epoll_lt.cpp` | `epoll_ctl(ADD)` 注册 → `epoll_wait` 取就绪列表；默认 LT |
| `04_epoll_et.cpp` | 比 LT 多 `EPOLLET`、非阻塞、循环读取直到 EAGAIN/EOF |
| `05_lt_et_probe.cpp` | 写 6 字节只读 2 字节，直接观察 LT/ET 的差别 |

例如编译 select：

```bash
g++ -std=c++17 01_select.cpp -o demo
./demo
```

替换文件名即可运行其他版本。前四个都会读出 `AB、CD、EF` 和 `12、34、56`，两条管道间的输出顺序可能不同；都故意每次只读 2 字节。

## 什么场景用什么？为什么？

| 场景 | 先考虑 | 为什么 |
| --- | --- | --- |
| 维护旧代码，只有很少且编号很低的 fd | select | 经典接口，但集合要反复构建，且 fd 编号受限 |
| 若干管道/socket，或数量适中的连接，需要 POSIX 可移植性 | poll | 数组直观，没有 FD_SETSIZE 限制；规模不大时逐项扫描通常可接受 |
| Linux 大量长连接，多数时间空闲 | epoll LT | 内核保存关注集合，每次返回就绪项，省去应用逐个检查空闲连接 |
| 已熟悉非阻塞，确实需要边沿触发语义 | epoll ET | 可以按事件批量处理，但必须排空，并注意繁忙连接占用处理时间 |

把“10000 个连接，只有 3 个有数据”代入：select/poll 返回后仍需检查关注项；epoll 直接给就绪项。因此**连接多、活跃比例低**时，epoll 的机制优势更明显。这不是本例测出的性能排名。

没有固定的“超过多少连接必须换 epoll”。若大多数连接都忙，大家都得处理这些数据；ET 也不保证比 LT 快，LT 同样可以循环读取。

## 记住 4 个坑就够了

1. **select 限制的是 fd 编号。** Linux/glibc 常见 FD_SETSIZE=1024，即使只监控一个 fd=1100 的 socket 也不行。`maxfd+1` 不是连接数。
2. **epoll 的结果数组长度不是连接上限。** `events[2]` 只表示一次最多取 2 个事件。
3. **HUP 不代表数据已经读完。** poll/epoll 看到写端关闭时，管道中仍可能有数据，读到 0 再关读端。
4. **就绪不是完成。** 仍要自己 read/write。本例是单读者管道；真实网络服务还需非阻塞、短写处理等，那是下一步的知识。

需要查接口细节再看：[select](https://man7.org/linux/man-pages/man2/select.2.html)、[poll](https://man7.org/linux/man-pages/man2/poll.2.html)、[epoll](https://man7.org/linux/man-pages/man7/epoll.7.html)。
