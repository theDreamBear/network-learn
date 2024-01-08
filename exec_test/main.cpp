#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <iostream>
#include <string>

bool isCommandSupport(std::string command) {
    if (command == "ls" || command == "pwd" || command == "ps" || command == "cat" || command == "grep" ||
        command == "wc" || command == "exit") {
        return true;
    }
    return false;
}

void childTask() {
    while (true) {
        // 获得一整行输入
        std::string line;
        size_t len = 0;
        std::getline(std::cin, line);
        // 进行分割
        char *p = strtok((char *) line.c_str(), " ");
        char *argv[100];
        int argc = 0;
        while (p != NULL) {
            argv[argc++] = p;
            p = strtok(NULL, " ");
        }
        argv[argc] = NULL;
        if (argc == 0) {
            continue;
        }
        if (isCommandSupport(argv[0])) {
            if (strcmp(argv[0], "exit") == 0) {
                exit(0);
            }
        } else {
            printf("command not support\n");
            continue;
        }
        int pipefd[2];
        pid_t pid;
        char buf;
        if (pipe(pipefd) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }

        pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {    /* 子进程 */
            close(pipefd[0]);          // 关闭读端
            dup2(pipefd[1], STDOUT_FILENO); // 将标准输出重定向到管道写端
            dup2(pipefd[1], STDERR_FILENO); // 将标准错误也重定向到管道写端
            close(pipefd[1]);          // 关闭原始写端
            execvp(argv[0], argv);
            _exit(EXIT_FAILURE);
        } else {          /* 父进程 */
            close(pipefd[1]);          // 关闭写端
            while (read(pipefd[0], &buf, 1) > 0) {
                write(STDOUT_FILENO, &buf, 1);
            }
            close(pipefd[0]);          // 关闭读端
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
            } else if (WIFSIGNALED(status)) {
                printf("child terminated abnormally, signal %d\n", WTERMSIG(status));
            }
        }
    }
}

int main() {
    childTask();
}