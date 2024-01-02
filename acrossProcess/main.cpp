#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>


int main() {
    int fds[2];
    int status = pipe(fds);
    if (status == -1) {
        printf("pipe error\n");
        return 1;
    }
    int pid = fork();
    if (pid == 0) {
        close(fds[1]);
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        while (true) {
            int len = read(fds[0], buffer, sizeof(buffer));
            if (len == 0) {
                printf("server close\n");
                break;
            } else if (len == -1) {
                printf("read error\n");
                break;
            } else {
                buffer[len] = '\0';
                std::cout << "read: " << buffer << std::flush;
            }
        }
    } else {
        close(fds[0]);
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        while (true) {
            printf("input: ");
            fgets(buffer, sizeof(buffer), stdin);
            if (strncmp(buffer, "quit", 4) == 0) {
                printf("quit\n");
                break;
            }
            int size = strlen(buffer);
            int len = 0;
            while (len < size) {
                int n = write(fds[1], buffer + len, size - len);
                if (n == -1) {
                    printf("write error\n");
                    break;
                }
                len += n;
            }
        }
    }
}
