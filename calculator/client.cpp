//
// Created by 倪超 on 2023/12/31.
//
#include <iostream>
// socket
#include <sys/socket.h>
// socketaddr_in
#include <netinet/in.h>
// inet_addr
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
// close
#include <unistd.h>
#include <vector>
#include "protocal.h"

class FdCloser {
public:
    int fd_;

    explicit FdCloser(int fd) : fd_(fd) {}

    ~FdCloser() {
        if (fd_ != -1) {
            close(fd_);
        }
    }

    void setFd(int fd) {
        fd_ = fd;
    }
};

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }
    FdCloser fdCloser(fd);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }
    printf("connect success\n");
    Data data;
    // 请输入数字个数
    std::cout << "请输入表达式: ";
    std::cin >> data.numSize;
    // 请输入数字
    //std::cout << "请输入数字: ";
    for (int i = 0; i < data.numSize; ++i) {
        int num;
        std::cin >> num;
        data.nums.push_back(num);
    }
    // 请输入操作符
    //std::cout << "请输入操作符: ";
    std::cin >> data.op;
    // 检验数据是否合法
    auto[code, msg] = isValid(data, 1024);
    if (code == -1) {
        std::cout << "数据不合法: " << msg << std::endl;
        return -1;
    }
    // 发送数据
    if (writeData(fd, data) == -1) {
        std::cout << "write data error" << std::endl;
        return -1;
    }
    // 接收数据
    int rd = 0;
    char buffer[1024];
    while (rd < sizeof(int)) {
        int n = read(fd, (char *) buffer + rd, sizeof(int) - rd);
        if (n == -1) {
            perror("read");
            return -1;
        }
        rd += n;
    }
    // decodeInt
    int result = 0;
    decodeInt(buffer, result);
    std::cout << "result: " << result << std::endl;
}

