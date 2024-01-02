//
// Created by 倪超 on 2023/12/29.
//
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == fd) {
        perror("socket");
        return 1;
    }
    // 初始化
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(1234);
    // 连接
    if (-1 == connect(fd, (struct sockaddr*)&client, sizeof(client))) {
        perror("connect");
        return 1;
    }
    // 发送数据
    const char *msg = "hello, server";
    auto n = write(fd, msg, strlen(msg));
    if (-1 == n) {
        perror("write");
        return 1;
    }
    // 读取数据
    char buf[1024];
    n = read(fd, buf, sizeof(buf));
    if (-1 == n) {
        perror("read");
        return 1;
    }
    n = std::min(1023, int(n));
    buf[n] = '\0';
    std::cout << "read: " << buf << std::endl;
    // 关闭连接
    close(fd);
}