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

int cal(const Data &data, int &result) {
    // 至少一个数字，一个运算符
    if (data.numSize <= 0 || data.numSize > 2) {
        std::cout << "numSize is invalid: " << data.numSize << std::endl;
        return -1;
    }
    // 一个数字, 只支持+ -
    if (data.numSize == 1) {
        result = data.nums[0];
        if (data.op != '+' && data.op != '-') {
            std::cout << "op is invalid: " << data.op << std::endl;
            return -1;
        }
        if (data.op == '-') {
            result = -result;
        }
        return 0;
    }
    // 2 个数字, 支持+ - * /
    if (data.numSize == 2) {
        if (data.op == '+') {
            result = data.nums[0] + data.nums[1];
        } else if (data.op == '-') {
            result = data.nums[0] - data.nums[1];
        } else if (data.op == '*') {
            result = data.nums[0] * data.nums[1];
        } else if (data.op == '/') {
            result = data.nums[0] / (data.nums[1] * 1.0);
        } else {
            std::cout << "op is invalid: " << data.op << std::endl;
            return -1;
        }
    }
    return 0;
}

int handleClient(int clientSd) {
    Conn conn(clientSd);
    Data data;
    if (readData(conn, data) == -1) {
        std::cout << "read data error" << std::endl;
        return -1;
    }
    std::cout << "numSize: " << data.numSize << std::endl;
    for (auto v: data.nums) {
        std::cout << "num: " << v << std::endl;
    }
    std::cout << "op: " << data.op << std::endl;
    // 计算结果
    int result;
    if (cal(data, result) == -1) {
        std::cout << "cal error" << std::endl;
        return -1;
    }
    std::cout << "result: " << result << std::endl;
    // 发送结果
    int wn = conn.writeInt(result);
    if (wn == -1) {
        std::cout << "write result error" << std::endl;
        return -1;
    }
    return 0;
}

int main() {
    auto sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        std::cout << "socket error" << std::endl;
        return -1;
    }
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sd, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        std::cout << "bind error" << std::endl;
        return -1;
    }
    if (listen(sd, 10) == -1) {
        std::cout << "listen error" << std::endl;
        return -1;
    }
    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        auto clientSd = accept(sd, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if (clientSd == -1) {
            std::cout << "accept error" << std::endl;
            continue;
        }
        // 打印客户端的ip和端口
        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientAddr.sin_addr, clientIp, sizeof(clientIp));
        std::cout << "client ip: " << clientIp << " port: " << ntohs(clientAddr.sin_port) << std::endl;
        handleClient(clientSd);
        close(clientSd);
    }
}
