#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "simple_protocol.h"

int readHeader(int fd) {
    int length = 0;
    int offset = 0;
    while (true) {
        char byte;
        auto n = read(fd, &byte, 1);
        if (n < 0) {
            perror("read");
            return -1;
        }
        if (n == 0) {
            return 0;
        }
        length |= (byte & 0x7f) << (offset);
        if (!(byte & 0x80)) {
            break;
        }
        offset += 7;
    }
    return length;
}

int readBody(int fd, int length) {
    char buffer[1024];
    int offset = 0;
    while (offset < length) {
        auto n = read(fd, buffer + offset, length - offset);
        if (n < 0) {
            perror("read");
            return -1;
        }
        if (n == 0) {
            return 0;
        }
        offset += n;
    }
    buffer[offset] = '\0';
    std::cout << "header:" << length << " body:" << buffer << std::endl;
    return offset;
}

int main() {
    auto sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        return 1;
    }
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = ntohl(INADDR_ANY);
    if (bind(sd, (sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    if (listen(sd, 10) < 0) {
        perror("listen");
        return 1;
    }
    while (true) {
        auto client = accept(sd, nullptr, nullptr);
        if (client < 0) {
            perror("accept");
            continue;
        }
        int length = readHeader(client);
        if (length < 0) {
            close(client);
            continue;
        }
        int rl = readBody(client, length);
        if (rl < 0) {
            close(client);
            continue;
        }
        close(client);
    }
}
