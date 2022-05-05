## socket 套接字
套接字（socket）是一个抽象层，应用程序可以通过它发送或接收数据，可对其进行像对文件一样的打开、读写和关闭等操作。套接字允许应用程序将I/O插入到网络中，并与网络中的其他应用程序进行通信。网络套接字是IP地址与端口的组合



<img src="https://media.geeksforgeeks.org/wp-content/uploads/udpfuncdiag.png" style="background-color: white;">
<img src="https://media.geeksforgeeks.org/wp-content/uploads/20220330131350/StatediagramforserverandclientmodelofSocketdrawio2-448x660.png" style="background-color: white;">

```
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
```


返回:
- fd 表示通信的一端
- -1: 出错，errno

参数:
- domain, family： 协议域/族, 指定了通信的”域”. 决定了socket的地址类型
    - AF_UNIX, AF_LOCAL: 本地通信。路径名作为地址
    - AF_INET: IPv4网络通信。ipv4地址（32位的）与端口号（16位的）的组合

- type：
    - SOCK_STREAM 流套接字。 sequenced, reliable, two-way, connection-based byte streams
    - SOCK_DGRAM 数据包套接字。connectionless, unreliable messages of a fixed maximum length
    - SOCK_RAW

- protocol: type 只有一种协议可为0

## bind
```
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```
绑定一个地址用于提供服务。服务端调用。

返回
- 0
- -1: 出错，errno

## listen
```
int listen(int sockfd, int backlog);
```

流套接字。监听。

## accept
```
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
流套接字。接收一个连接请求，新建连接套接字。非阻塞socket会等待连接请求

返回：
- fd: 连接套接字
- -1: 出错，errno。
    - EAGAIN

参数
- sockfd: 监听套接字
- addr: 对端地址

## connect
```
int connect(int sockfd, const struct sockaddr *addr,
            socklen_t addrlen);
```
客户端发送连接请求

udp 也可以 connect, 但还是无连接协议, 只能与单个server通信, 无 server 进行 send 可能出现 ECONNREFUSED 错误

返回
- 0
- -1: 出错，errno
    - ECONNREFUSED

参数
- sockfd：客户端的socket
- addr：服务器的socket地址


## read write
```
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
```
文件读写

读buf不够大，根据不同的套接字类型可能会丢弃剩余数据

- sockfd: 连接套接字

返回
- 字节数
- -1: 出错，errno。
    - EAGAIN/EWOULDBLOCK: 非阻塞socket无数据可读写

## recv send
```
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```
读buf不够大，根据不同的套接字类型可能会丢弃剩余数据

- sockfd: 连接套接字
- flags:
    - MSG_DONTWAIT: 非阻塞，同fcntl(O_NONBLOCK)
    - MSG_WAITALL: 对udp无效

## recvfrom, sendto
```
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags,
                 struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
```

`recv(sockfd, buf, len, flags);` <==> `recvfrom(sockfd, buf, len, flags, NULL, NULL);`


> 参考文献
- https://www.geeksforgeeks.org/socket-programming-cc
- https://www.geeksforgeeks.org/udp-server-client-implementation-c/
- https://blog.csdn.net/zwz2011303359/article/details/81809910
- https://swamy2064.wordpress.com/2018/12/01/can-udp-use-connect
- man 2/3