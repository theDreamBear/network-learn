#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

int test_wait() {
    pid_t pid = fork();
    if (pid == 0) {
        printf("child process\n");
    } else {
        sleep(3);
        printf("parent process\n");
    }
    if (pid == 0) {
        printf("end child process\n");
        exit(12);
    } else {
        int status;
        wait(&status);
        if (WIFEXITED(status)) {
            printf("child process exit with %d\n", WEXITSTATUS(status));
        } else {
            printf("child process exit abnormally %d\n", WEXITSTATUS(status));
        }
        printf("end parent process\n");
    }
    return 0;
}

int test_waitpid() {
    pid_t  pid = fork();
    if (pid == 0) {
        printf("child process\n");
        sleep(3);
    } else {
        printf("parent process\n");
        int status;
        while (!waitpid(pid, &status, WNOHANG)) {
            printf("wait child process\n");
            sleep(1);
        }
        if (WIFEXITED(status)) {
            printf("child process exit with %d\n", WEXITSTATUS(status));
        } else {
            printf("child process exit abnormally %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}

void mtimeout(int sig) {
    if (sig == SIGALRM) {
        printf("timeout\n");
    }
    alarm(2);
}

void mkey_control(int sig) {
    if (sig == SIGINT) {
        printf("key control\n");
    }
}

void mhandle_child(int sig) {
    if (sig == SIGCHLD) {
        printf("handle child\n");
        int status;
        waitpid(-1, &status, WNOHANG);
        if (WIFEXITED(status)) {
            printf("child process exit with %d\n", WEXITSTATUS(status));
        } else {
            printf("child process exit abnormally %d\n", WEXITSTATUS(status));
        }
    }
}
class TimeElapse {
private:
    struct timeval begin, end;

public:
    void start() {
        gettimeofday(&begin, NULL);
    }

    void stop() {
        gettimeofday(&end, NULL);
    }

    int elapse() {
        return (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);
    }
};


void sleep_n(int n) {
    int left = n * 1000000;
    TimeElapse timeElapse;
    while (left > 0) {
        timeElapse.start();
        usleep(left);
        timeElapse.stop();
        std::cout << "sleep " << timeElapse.elapse() / 1000 << "ms" << std::endl;
        left -= timeElapse.elapse();
    }
}

void test_signal() {
    signal(SIGALRM, mtimeout);
    signal(SIGINT, mkey_control);
    signal(SIGCHLD, mhandle_child);
    alarm(2);
    pid_t  pid = fork();
    if (pid == 0) {
        printf("child process\n");
        sleep(3);
    } else {
        printf("parent process\n");
        sleep_n(5);
        printf("end parent process\n");
    }
}

int main() {
    struct sigaction child;
    child.sa_handler = mhandle_child;
    sigemptyset(&child.sa_mask);
    child.sa_flags = 0;
    sigaction(SIGCHLD, &child, NULL);

    pid_t  pid = fork();
    if (pid == 0) {
        printf("child process\n");
        sleep(3);
        exit(1);
    } else {
        pid = fork();
        if (pid == 0) {
            printf("child process\n");
            sleep(3);
            exit(2);
        } else {
            printf("parent process\n");
            sleep_n(10);
            printf("end parent process\n");
        }
    }
}
