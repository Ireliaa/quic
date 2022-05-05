# epoll
由3个系统调用组成：epoll_create, epoll_ctl and epoll_wait。支持大量文件，性能不随数量增加而线性下降

## epoll_create

```
#include <sys/epoll.h>
int epoll_create(int size);
int epoll_create1(int flags);
```

新建epoll实例，返回文件描述符，用`close`释放。后续调用在该实例上

- size: 大于0。FD_CLOEXEC

返回
- 文件描述符， 非负
- -1: 出错，errno

## epoll_ctl

```
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
```

返沪：
- 0
- -1: 错误，errno

参数

- epfd: epoll 实例
- op:
    - `EPOLL_CTL_ADD`: 注册 fd，关联事件
    - `EPOLL_CTL_MOD`: 修改fd 关联的事件
    - `EPOLL_CTL_DEL`: 移除fd, event忽略
- fd: 要操作的目标文件描述符
- event

```
typedef union epoll_data {
    void        *ptr;
    int          fd;
    uint32_t     u32;
    uint64_t     u64;
} epoll_data_t;

struct epoll_event {
    uint32_t     events;      /* Epoll events */
    epoll_data_t data;        /* User data variable */
};
```


epoll_event.events:
- EPOLLIN: fd 可读
- EPOLLOUT: fd 可写
- EPOLLRDHUP: socket 关闭连接
- EPOLLET: Edge Triggered，边缘触发，状态发生变化时。只支持no-block socket

## epoll_wait
```
int epoll_wait(int epfd, struct epoll_event *events,
               int maxevents, int timeout);
```
等待（多个fd）事件

- events(OUT): 返回可用事件
- maxevents: >0
- timeout: 超时毫秒数，-1 一直等, 0 立即返回

返回：
- 文件描述符数量
- 0: 超时，无事件
- -1: 出错，errno




> 参考文献
- man 2/3