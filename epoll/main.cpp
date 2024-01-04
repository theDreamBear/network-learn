#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

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
    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Error binding socket");
        return 1;
    }
    if (listen(sd, 5) == -1) {
        perror("Error listening on socket");
        return 1;
    }
    int efd = epoll_create1(EPOLL_CLOEXEC);
    if (efd == -1) {
        perror("Error creating epoll");
        return 1;
    }
    struct epoll_event event;
    event.data.fd = sd;
    event.events = EPOLLIN;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, sd, &event) == -1) {
        perror("Error adding socket to epoll");
        return 1;
    }
    int MAX_EVENTS = 100;
    struct epoll_event *events =
        (struct epoll_event *)malloc(sizeof(struct epoll_event) * MAX_EVENTS);
    while (1) {
        int epoll_cnt = epoll_wait(efd, events, MAX_EVENTS, -1);
        if (epoll_cnt == -1) {
            perror("Error waiting for events");
            return 1;
        }
        for (int i = 0; i < epoll_cnt; i++) {
            if (events[i].data.fd == sd) {
                struct sockaddr_in client;
                socklen_t client_len = sizeof(client);
                int client_sd =
                    accept(sd, (struct sockaddr *)&client, &client_len);
                if (client_sd == -1) {
                    perror("Error accepting connection");
                    return 1;
                }
                int flags = fcntl(client_sd, F_GETFL, 0);
                if (flags == -1) {
                    perror("Error getting socket flags");
                    return 1;
                }
                flags |= O_NONBLOCK;
                if (fcntl(client_sd, F_SETFL, flags) == -1) {
                    perror("Error setting socket flags");
                    return 1;
                }
                char client_ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(client.sin_addr), client_ip,
                          INET_ADDRSTRLEN);
                printf("Connection accepted from %s:%d\n", client_ip,
                       ntohs(client.sin_port));
                event.data.fd = client_sd;
                event.events = EPOLLIN | EPOLLET;
                if (epoll_ctl(efd, EPOLL_CTL_ADD, client_sd, &event) == -1) {
                    perror("Error adding socket to epoll");
                    return 1;
                }
            } else {
                while (1) {
                    char buffer[1024];
                    int bytes_read = read(events[i].data.fd, buffer, 1024);
                    if (bytes_read > 0) {
                        printf("Received %d bytes: %s\n", bytes_read, buffer);
                        for (int i = 0; i < bytes_read; i++) {
                            buffer[i] = toupper(buffer[i]);
                        }
                        if (write(events[i].data.fd, buffer, bytes_read) == -1) {
                            perror("Error writing to socket");
                            return 1;
                        }
                        continue;
                    }
                    if (bytes_read == -1) {
                        if (errno == EAGAIN) {
                            break;
                        }
                        perror("Error reading from socket");
                    }
                    printf("Connection closed\n");
                    if (epoll_ctl(efd, EPOLL_CTL_DEL, events[i].data.fd,
                                  NULL) == -1) {
                        perror("Error removing socket from epoll");
                        return 1;
                    }
                    close(events[i].data.fd);
                }
            }
        }
    }
}
