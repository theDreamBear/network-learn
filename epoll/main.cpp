#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/errno.h>

int main() {
  int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("Error creating socket");
        return 1;
    }
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("Error binding socket");
        return 1;
    }
    if (listen(sd, 5) == -1) {
        perror("Error listening on socket");
        return 1;
    }
    int efd = epoll_create(100);
    if (efd == -1) {
        perror("Error creating epoll");
        return 1;
    }
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = sd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sd, &event) == -1) {
        perror("Error adding socket to epoll");
        return 1;
    }
}
