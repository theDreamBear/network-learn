//
// Created by 倪超 on 2023/12/30.
//
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include "simple_protocol.h"
#include <cstring>

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
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }
    char buffer[1024];
    std::string data;
    getline(std::cin, data);
    int length = strlen(data.c_str());
    int newLen = makeHeader((void*)buffer, length);
    snprintf(buffer + newLen, sizeof(buffer) - newLen, "%s", data.c_str());
    int wl = write(sd, buffer, newLen + length);
    close(sd);
}
