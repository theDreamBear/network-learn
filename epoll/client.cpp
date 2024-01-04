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
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Error connecting to server");
        return 1;
    }
    char buffer[1024];
    while (1) {
        bzero(buffer, sizeof(buffer));
        std::cin.getline(buffer, sizeof(buffer));
        if (send(sd, buffer, sizeof(buffer), 0) == -1) {
            perror("Error sending message");
            return 1;
        }
        if (strcmp(buffer, "exit") == 0) {
            break;
        }
        bzero(buffer, sizeof(buffer));
        if (recv(sd, buffer, sizeof(buffer), 0) == -1) {
            perror("Error receiving message");
            return 1;
        }
        std::cout << buffer << std::endl;
    }
}