#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <time.h>

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    fd_set readfds;
    char buffer[512];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    int cnt = 0;
    FILE* fp = fdopen(sockfd, "r");
    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        // 问题是，
        if (select(sockfd + 1, &readfds, NULL, NULL, NULL) > 0) {
            printf("select\n");
            if (FD_ISSET(sockfd, &readfds)) {
                if (fgets(buffer, sizeof(buffer), fp) != NULL) {
                    time_t t = time(NULL);
                    // 时间格式化 到秒
                    char* p = ctime(&t);
                    // 去掉换行符
                    p[strlen(p) - 1] = '\0';
                    printf("[%s] %d\n", p, strlen(buffer));
                    cnt += strlen(buffer);
                    // 可以继续处理其他数据
                } else {
                    perror("fgets");
                    break;
                }
            }
        }
    }
    printf("cnt: %d\n", cnt);
    close(sockfd);
    return 0;
}
