//
// Created by 倪超 on 2023/12/31.
//

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

void read_process(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while (true) {
        int len = read(fd, buffer, sizeof(buffer));
        if (len == 0) {
            printf("server close\n");
            break;
        } else if (len == -1) {
            printf("read error\n");
            break;
        } else {
            buffer[len] = '\0';
            std::cout << "read: " << buffer << std::flush;
        }
    }
    close(fd);
}

void write_process(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while (true) {
        printf("input: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (strncmp(buffer, "quit", 4) == 0) {
            printf("quit\n");
            shutdown(fd, SHUT_WR);
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
    }
    close(fd);
}

int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cout << "socket() failed\n";
        return 1;
    }
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    auto state = bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (state == -1) {
        std::cout << "bind() failed\n";
        return 1;
    }
    state = listen(fd, 10);
    if (state == -1) {
        std::cout << "listen() failed\n";
        return 1;
    }
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        auto client_fd = accept(fd, (struct sockaddr*)&client_addr, &len);
        if (client_fd == -1) {
            std::cout << "accept() failed\n";
            continue;
        }
        char buffer[INET_ADDRSTRLEN];
        std::cout << "client ip: " << inet_ntop(AF_INET, &client_addr.sin_addr, buffer, sizeof(buffer)) << std::endl;
        std::cout << "client port: " << ntohs(client_addr.sin_port) << std::endl;
        pid_t pid = fork();
        if (pid == 0) {
            close(fd);
            read_process(client_fd);
            return 0;
        } else {
            write_process(client_fd);
        }
    }
    close(fd);
}