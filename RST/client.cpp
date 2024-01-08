#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8111);
    inet_pton(AF_INET, "10.116.32.38", &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connect failed");
        exit(1);
    }

    // 尝试读取数据
    int n = read(sockfd, buffer, 255);
    if (n < 0) {
        perror("Read failed");
    } else if (n == 0) {
        perror("connection closed by server");
    } else {
        printf("Received data: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
