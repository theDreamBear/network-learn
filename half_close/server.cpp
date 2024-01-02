#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void handleClient(int clientFd) {
    FILE* file = fopen("../CMakeLists.txt", "r");
    if (file == nullptr) {
        perror("fopen");
        return;
    }
    char buffer[1024];
    while (true) {
        int n = fread(buffer, 1, sizeof(buffer), file);
        if (n < 0) {
            perror("fread");
            break;
        }
        if (n == 0) {
            break;
        }
        write(clientFd, buffer, n);
    }
    // half close
    shutdown(clientFd, SHUT_WR);
    // read
    while (true) {
        int n = read(clientFd, buffer, sizeof(buffer));
        if (n < 0) {
            perror("read");
            break;
        }
        if (n == 0) {
            break;
        }
        buffer[n] = '\n';
        buffer[n + 1] = '\0';
        // 输出
        fputs(buffer, stdout);
    }
    fclose(file);
}

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
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int ret = bind(fd, (struct sockaddr *) &addr, sizeof(addr));
    if (ret == -1) {
        perror("bind");
        return -1;
    }
    ret = listen(fd, 10);
    if (ret == -1) {
        perror("listen");
        return -1;
    }
    while (true) {
        struct sockaddr_in clientAddr;
        socklen_t len = sizeof(clientAddr);
        auto clientFd = accept(fd, (struct sockaddr *) &clientAddr, &len);
        if (clientFd == -1) {
            perror("accept");
            return -1;
        }
        char ip[INET_ADDRSTRLEN];
        printf("client ip: %s, port: %d\n", inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip)),
               ntohs(clientAddr.sin_port));
        handleClient(clientFd);
        close(clientFd);
    }
}
