/*
    测试:
    - 运行本程序。输入q回车退出
    - nc -u 127.0.0.1 port 后输入字符回车
 */
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <iostream>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

using namespace std;

#define MAX_EVENTS 100
#define MAX_READ 1024


#define EP_ENSURE(EXPR, ...) EP_CHECK(EXPR, return __VA_ARGS__)
#define EP_WARN(EXPR, ...) EP_CHECK(EXPR)
#define EP_CHECK(EXPR, ...) \
    if ((EXPR) == -1) { \
        printf("ERROR " #EXPR " (%d): %s\n", errno, strerror(errno)); \
        __VA_ARGS__; \
    }


void run(int listen_fd)
{
    while (true) {
        char buf[MAX_READ]{};
        sockaddr_storage c_addr{};
        socklen_t len = sizeof(c_addr);
        auto ret = recvfrom(listen_fd, &buf, sizeof(buf), 0, (sockaddr*)&c_addr, &len);
        if (ret <= 0) {
            printf("ret = %ld, error: %s\n", ret, strerror(errno));
        } else {
            char hbuf[NI_MAXHOST]{}, sbuf[NI_MAXSERV]{};
            getnameinfo((sockaddr*)&c_addr, len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
            printf("%ld from %s:%s: %s\n", ret, hbuf, sbuf, buf);
            if (ret == 2 && buf[0] == 'q' && buf[1] == '\n') {
                break;
            }
        }
    }
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

    int listen_fd = -1;
    EP_ENSURE(listen_fd = socket(AF_INET, SOCK_DGRAM, 0), EXIT_FAILURE);

    struct cleanup_t {
        int fd_ = -1;
        cleanup_t(int& fd): fd_(fd) {}
        ~cleanup_t() { close(fd_);}
    } cleanup(listen_fd);

    int reuse = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    //setsockopt(listen_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
    // 把socket设置为阻塞方式
    int opts = 0;
    EP_ENSURE(opts = fcntl(listen_fd, F_GETFL), EXIT_FAILURE);
    EP_ENSURE(fcntl(listen_fd, F_SETFL, opts & ~O_NONBLOCK), EXIT_FAILURE);
    //
    sockaddr_in sv_addr{};
    sv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &sv_addr.sin_addr);
    //sv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // INADDR_ANY
    sv_addr.sin_port = htons(port);
    EP_ENSURE(::bind(listen_fd, (sockaddr*)&sv_addr, sizeof(sv_addr)), EXIT_FAILURE);

    thread t([&]{
        run(listen_fd);
    });

    char line[512];
    while (true) {
        if (fgets(line, sizeof(line), stdin)) {
            sendto(listen_fd, line, strlen(line), 0, (sockaddr*)&sv_addr, sizeof(sv_addr));
            if (line[0] == 'q' && line[1] == '\n') {
                break;
            }
        }
    }

    t.join();
    return 0;
}