#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(1234);

    auto sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == sd) {
        perror("socket");
        return 1;
    }
    char buf[1024];
    if (-1 == bind(sd, (struct sockaddr*)&server, sizeof(server))) {
        perror("bind");
        return 1;
    }
    while (1) {
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        auto n = recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr*)&client, &len);
        if (-1 == n) {
            perror("recvfrom");
            continue;
        }
        n = std::max(1023, int(n));
        buf[n] = '\0';
        // client信息
        char ip[INET_ADDRSTRLEN];
        auto p = inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        if (nullptr == p) {
            perror("inet_ntop");
            continue;
        }
        std::cout << "client ip: " << ip << ", port: " << ntohs(client.sin_port) << std::endl;
        std::cout << "read: " << buf << std::endl;
        ssize_t  wl = snprintf(buf, sizeof(buf), "hello, client");
        n = sendto(sd, buf, wl, 0, (struct sockaddr*)&client, len);
        if (-1 == n) {
            perror("sendto");
            continue;
        }
    }
}
