project(hello)

add_executable(hello src/main.cpp)

# 链接依赖 libpthread.so
target_link_libraries(hello PRIVATE pthread)

add_executable(udpsvr src/udp_sever.cpp)
add_executable(udpcli src/udp_client.cpp)
add_executable(udpsvrmt src/udp_sever_thread.cpp)
target_link_libraries(udpsvrmt PRIVATE pthread)


add_executable(tcpsvr src/tcp_sever.cpp)
add_executable(tcpcli src/tcp_client.cpp)
