//
// Created by 倪超 on 2024/1/1.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <mutex>

int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cout << "socket() failed\n";
        return 1;
    }
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int status = connect(fd, (sockaddr*)&addr, sizeof(addr));
    if (status == -1) {
        std::cout << "connect() failed\n";
        return 1;
    }
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while (true) {
        printf("input: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (strncmp(buffer, "quit", 4) == 0) {
            printf("quit\n");
            break;
        }
        int size = strlen(buffer);
        int len = 0;
        while (len < size) {
            int n = write(fd, buffer + len, size - len);
            if (n == -1) {
                printf("write error\n");
                break;
            }
            len += n;
        }
        printf("write: %s\n", buffer);
    }
}