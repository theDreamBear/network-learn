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

int read_with_timeout(int fd, char* buffer, int size, int timeout) {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    struct timeval tv;
    tv.tv_sec = timeout / 1000000;
    tv.tv_usec = timeout % 1000000;
    int n = select(fd + 1, &readfds, nullptr, nullptr, &tv);
    if (n == -1) {
        perror("select");
        return -1;
    } else if (n == 0) {
        printf("read timeout\n");
        return 0;
    }
    int len = 0;
    while (len < size) {
        int n = read(fd, buffer + len, size - len);
        if (n == -1) {
            perror("read");
            return -1;
        } else if (n == 0) {
            printf("read eof\n");
            return 0;
        }
        len += n;
    }
    return len;
}

int main() {
   auto sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        std::cout << "socket() failed\n";
        return 1;
    }
    int opt = 1;
    int status = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (status == -1) {
        std::cout << "setsockopt() failed\n";
        return 1;
    }
    sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    status = bind(sd, (sockaddr*)&addr, sizeof(addr));
    if (status == -1) {
        std::cout << "bind() failed\n";
        return 1;
    }
    status = listen(sd, 10);
    if (status == -1) {
        std::cout << "listen() failed\n";
        return 1;
    }
    printf("server start %d\n", sd);
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sd, &readfds);
    int maxfd = sd;
    while (true) {
        fd_set tmpfds = readfds;
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 10000;
        int n = select(maxfd + 1, &tmpfds, nullptr, nullptr, &tv);
        if (n == -1) {
            perror("select");
            return 1;
        } else if (n == 0) {
            continue;
        }
        for (int i = 0; i < maxfd + 1; ++i) {
            if (FD_ISSET(i, &tmpfds)) {
                if (i == sd) {
                    // accept
                    struct sockaddr_in client_addr;
                    socklen_t len = sizeof(client_addr);
                    auto fd = accept(sd, (sockaddr*)&client_addr, &len);
                    if (fd == -1) {
                        perror("accept");
                        return 1;
                    }
                    char buffer[INET_ADDRSTRLEN];
                    memset(buffer, 0, sizeof(buffer));
                    inet_ntop(AF_INET, &client_addr.sin_addr, buffer, sizeof(buffer));
                    printf("accept client %s:%d fd:%d\n", buffer, ntohs(client_addr.sin_port), fd);
                    FD_SET(fd, &readfds);
                    if (fd > maxfd) {
                        maxfd = fd;
                    }
                } else {
                    // io
                    char buffer[1024];
                    memset(buffer, 0, sizeof(buffer));
                    int len = read(i, buffer, sizeof(buffer));
                    if (len == 0) {
                        printf("client close %d\n", i);
                        close(i);
                        FD_CLR(i, &readfds);
                    } else if (len == -1) {
                        perror("read");
                        return 1;
                    } else {
                        printf("read: %s\n", buffer);
                    }
                }
            }
        }
    }
}
