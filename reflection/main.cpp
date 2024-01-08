#include <iostream>
#include "net.h"
#include <sys/wait.h>
#include <signal.h>

void sig_child(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
        std::cout << "child " << pid << " terminated" << std::endl;
    }
    return;
}

void handle_con(int conn) {
    char buf[1024] = {0};
    for (int i = 0; i < 1022; ++i) {
        buf[i] = 'a';
    }
    buf[1022] = '\n';
    buf[1023] = '\0';
    int cnt = 0;
    for (int i = 0; i < 10; i++) {
        writen(conn, buf, strlen(buf));
        cnt += strlen(buf);
    }
    sleep(10);
    close(conn);
    abort();
    printf("cnt: %d\n", cnt);
}

int main() {
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int listenfd = Socket();
    Bind(listenfd, server);
    Listen(listenfd);
    struct sigaction sig;
    sig.sa_handler = sig_child;
    sigemptyset(&sig.sa_mask);
    sig.sa_flags = 0;

    int option = 1;
    int status = setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &status, sizeof(status));
    if (status < 0) {
        Perrorf("setsockopt error");
        exit(-1);
    }
    while (1) {
        struct sockaddr_in client;
        int conn = Accept(listenfd, &client);
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, sizeof(ip));
        std::cout << "client ip: " << ip << " port: " << ntohs(client.sin_port) << std::endl;
        pid_t pid = fork();
        if (pid < 0) {
            Perrorf("fork error");
            exit(-1);
        } else if (pid == 0) {
            close(listenfd);
            handle_con(conn);
            exit(0);
        } else {
            close(conn);
        }
    }
}
