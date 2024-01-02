//
// Created by 倪超 on 2023/12/31.
//
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    // 8080
    addr.sin_port = htons(8080);
    //
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int ret = connect(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("connect");
        return -1;
    }
    printf("connect success\n");
    FILE* file = fopen("./CMakeLists-read.txt", "w");
    if (file == nullptr) {
        perror("fopen");
        return -1;
    }
    char buffer[1024];
    ret = 0;
    while (true) {
        int n = read(fd, buffer, sizeof(buffer));
        if (n < 0) {
            perror("read");
            ret = -1;
            break;
        }
        if (n == 0) {
            break;
        }
        fwrite(buffer, 1, n, file);
    }
    if (ret) {
        fclose(file);
        close(fd);
        return -1;
    }
    // send thank you
    const char* thankYou = "thank you";
    write(fd, thankYou, strlen(thankYou));
    fclose(file);
    close(fd);
}
