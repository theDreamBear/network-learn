#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <signal.h>

void process_child(int sig) {
    if (sig == SIGCHLD) {
        printf("child process exit\n");
        int status;
        waitpid(-1, &status, WNOHANG);
        if (WIFEXITED(status)) {
            printf("child process exit with %d\n", WEXITSTATUS(status));
        } else {
            printf("child process exit abnormally %d\n", WEXITSTATUS(status));
        }
    }
}

void read_process(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while (true) {
        int len = read(fd, buffer, sizeof(buffer));
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
    close(fd);
}

void write_process(int fd) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    while (true) {
        printf("input: ");
        fgets(buffer, sizeof(buffer), stdin);
        if (strncmp(buffer, "quit", 4) == 0) {
            printf("quit\n");
            shutdown(fd, SHUT_WR);
            break;
        }
        int size = strlen(buffer);
        int len = 0;
        while (len < size) {
            int n = write(fd, buffer + len, size - len);
            if (n == -1) {
                printf("write error\n");
                break;
            }
            len += n;
        }
    }
    close(fd);
}



int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cout << "socket() failed\n";
        return 1;
    }
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(8080);
    client_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int state = connect(fd, (struct sockaddr*)&client_addr, sizeof(client_addr));
    if (state == -1) {
        std::cout << "connect() failed\n";
        return 1;
    }
    pid_t pid = fork();
    if (pid == 0) {
        read_process(fd);
    } else {
        write_process(fd);
    }
}
