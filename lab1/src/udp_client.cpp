/*
    测试:
    - 启动服务端 nc -ul 5555, 或启动本项目的udpsvr
    - 运行本程序。输入字符回车退出
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <string>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

#define UDP_CONNECT 0
#define MAX_EVENTS 100
#define MAX_READ 1024


#define EP_ENSURE(EXPR, ...) EP_CHECK(EXPR, return __VA_ARGS__)
#define EP_WARN(EXPR, ...) EP_CHECK(EXPR)
#define EP_CHECK(EXPR, ...) \
    if ((EXPR) == -1) { \
        printf("ERROR " #EXPR " (%d): %s\n", errno, strerror(errno)); \
        __VA_ARGS__; \
    }

int main(int argc, const char** argv)
{
    const char* ip = "127.0.0.1";
    uint16_t port = 5555;
    if (argc >= 2) {
        ip = argv[1];
        if (argc >= 3)
            port = atoi(argv[2]);
    }

    int cfd = 0;
    EP_ENSURE(cfd = socket(AF_INET, SOCK_DGRAM, 0), EXIT_FAILURE);

    int reuse = 1;
    setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    //setsockopt(cfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    // 把socket设置为非阻塞方式
    int opts = 0;
    EP_ENSURE(opts = fcntl(cfd, F_GETFL), EXIT_FAILURE);
    EP_ENSURE(fcntl(cfd, F_SETFL, opts | O_NONBLOCK), EXIT_FAILURE);
//
    sockaddr_in sv_addr{};
    sv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sv_addr.sin_addr);
    //sv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // INADDR_ANY
    sv_addr.sin_port = htons(port);
#if (UDP_CONNECT + 0)
    EP_ENSURE(connect(cfd, (sockaddr*)&sv_addr, sizeof(sv_addr)), EXIT_FAILURE);
#endif

    int efd = -1;
    EP_ENSURE(efd = epoll_create1(EPOLL_CLOEXEC), EXIT_FAILURE);
    epoll_event ev{};
    //设置与要处理的事件相关的文件描述符
    // 监听标准输入
    ev.data.fd = STDIN_FILENO;
    //设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET;
    //注册epoll事件
    EP_ENSURE(epoll_ctl(efd, EPOLL_CTL_ADD, STDIN_FILENO, &ev), EXIT_FAILURE);

    epoll_event events[MAX_EVENTS]{};
    while (true) {
        int nfds = epoll_wait(efd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            printf("epoll_wait error: %s\n", strerror(errno));
            break;
        }
        for (int i = 0; i < nfds; ++i) {
            auto fd = events[i].data.fd;
            if (events[i].events & EPOLLIN && fd == STDIN_FILENO) {
                string s;
                cin >> s;
                if (s == "q")
                    goto end;
#if (UDP_CONNECT + 0)
                EP_ENSURE(write(cfd, s.data(), s.size()), EXIT_FAILURE);
#else
                EP_ENSURE(sendto(cfd, s.data(), s.size(), 0, (sockaddr*)&sv_addr, sizeof(sv_addr)), EXIT_FAILURE);
#endif
            }
        }
    }
end:
    close(cfd);
    close(efd);
    return 0;
}