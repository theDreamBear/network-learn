//
// Created by 倪超 on 2024/1/7.
//

#ifndef SIGIO_NET_H
#define SIGIO_NET_H
//
// Created by 倪超 on 2024/1/6.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define Perrorf(str) { \
    char buffer[1024] = {0}; \
    snprintf(buffer, sizeof(buffer), "%s line[%d], func[%s]", str, __LINE__, __func__); \
    perror(buffer); \
}

int readn(int sock, char* buf, int len) {
    int readSize = 0;
    while (readSize < len) {
        int ret = read(sock, buf + readSize, len - readSize);
        if (ret < 0) {
            Perrorf("read error");
            return -1;
        } else if (ret == 0) {
            return 0;
        }
        readSize += ret;
    }
    return readSize;
}

int writen(int sock, char* buf, int len) {
    int writeSize = 0;
    while (writeSize < len) {
        int ret = write(sock, buf + writeSize, len - writeSize);
        if (ret < 0) {
            Perrorf("write error");
            return -1;
        } else if (ret == 0) {
            return 0;
        }
        writeSize += ret;
    }
    return writeSize;
}

int Socket(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        Perrorf("socket error");
        exit(-1);
    }
    return sock;
}

void Connect(int& sock, struct sockaddr_in& addr) {
    int ret = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        Perrorf("connect error");
        exit(-1);
    }
}

void Bind(int& sock, struct sockaddr_in& addr) {
    int ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        Perrorf("bind error");
        exit(-1);
    }
}

void Listen(int& sock) {
    int ret = listen(sock, 5);
    if (ret < 0) {
        Perrorf("listen error");
        exit(-1);
    }
}

int Accept(int& sock, struct sockaddr_in *client_addr) {
    socklen_t len = sizeof(*client_addr);
    int client_sock = accept(sock, (struct sockaddr*)client_addr, &len);
    if (client_sock < 0) {
        Perrorf("accept error");
        return -1;
    }
    return client_sock;
}

#endif //SIGIO_NET_H
