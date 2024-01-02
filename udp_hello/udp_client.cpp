//
// Created by 倪超 on 2023/12/30.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <algorithm>
int main() {
    auto fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == fd) {
        perror("socket");
    }
    struct sockaddr_in client;
    bzero(&client, sizeof(client));
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = inet_addr("127.0.0.1");
    client.sin_port = htons(1234);

    char buf[1024];
    int wl = snprintf(buf, sizeof(buf), "%s", "hello server");
    int len = sendto(fd, buf, wl, 0, (struct sockaddr*)&client, sizeof(client));
    if (-1 == len) {
        perror("sendto");
        return 1;
    }
    len = recvfrom(fd, buf, sizeof(buf), 0, nullptr, nullptr);
    if (-1 == len) {
        perror("recvfrom");
        return 1;
    }
    auto rl = std::max(1023, int(len));
    buf[rl] = '\0';
    printf("read: %s\n", buf);
}
