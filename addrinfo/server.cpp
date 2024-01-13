#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>

using namespace std;

int tcp_listen(const char* host, const char* server, struct sockaddr ** saddr, socklen_t* len) {
   struct addrinfo hints, *res, *save;
    bzero(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int ret = getaddrinfo(host, server, &hints, &res);
    if (ret != 0) {
        printf("getaddrinfo: %s %d\n", gai_strerror(ret), ret);
        return -1;
    }
    save = res;
    int fd = -1;
    for (; res != NULL; res = res->ai_next) {
        if (res->ai_family == AF_INET) {
            char buf[INET_ADDRSTRLEN];
            struct sockaddr_in* addr = (struct sockaddr_in*)res->ai_addr;
            printf("ip: %s port: %d\n", inet_ntop(AF_INET, &addr->sin_addr, buf, INET_ADDRSTRLEN), ntohs(addr->sin_port));
        } else if (res->ai_family == AF_INET6) {
            char buf[INET6_ADDRSTRLEN];
            struct sockaddr_in6* addr = (struct sockaddr_in6*)res->ai_addr;
            printf("ip: %s port: %d\n", inet_ntop(AF_INET6, &addr->sin6_addr, buf, INET6_ADDRSTRLEN), ntohs(addr->sin6_port));
        }
        fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (fd == -1) {
            printf("socket: %s\n", strerror(errno));
            continue;
        }
        int opt = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            printf("setsockopt: %s\n", strerror(errno));
            close(fd);
            continue;
        }
        if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
            printf("bind: %s\n", strerror(errno));
            close(fd);
            continue;
        }
        if (listen(fd, 5) == -1) {
            printf("listen: %s\n", strerror(errno));
            close(fd);
            continue;
        }
        break;
    }
    if (res == NULL) {
        printf("failed to bind\n");
        return -1;
    }
    if (saddr != NULL) {
        *len = res->ai_addrlen;
        *saddr = (struct sockaddr*)malloc(res->ai_addrlen);
        memcpy(*saddr, res->ai_addr, res->ai_addrlen);
    }
    freeaddrinfo(save);
    return fd;
}

int main() {
    struct sockaddr* saves;
    socklen_t len;
    if (tcp_listen("127.0.0.1", "12345", &saves, &len) == -1) {
        return -1;
    }
}