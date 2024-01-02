#include <iostream>
#include <sys/socket.h>
int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cout << "socket() failed\n";
        return 1;
    }
    int rvb = 1024 * 1024;
    auto state = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char*)&rvb, sizeof(rvb));
    if (state == -1) {
        std::cout << "setsockopt() failed\n";
        return 1;
    }
    state = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char*)&rvb, sizeof(rvb));
    if (state == -1) {
        std::cout << "setsockopt() failed\n";
        return 1;
    }
    int len;
    state = getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &rvb, (socklen_t*)&len);
    if (state == -1) {
        std::cout << "getsockopt() failed\n";
        return 1;
    }
    // 输出
    std::cout << "SO_RCVBUF: " << rvb / 1024 << std::endl;

    int sb;
    state = getsockopt(fd, SOL_SOCKET, SO_SNDBUF, &sb, (socklen_t*)&len);
    if (state == -1) {
        std::cout << "getsockopt() failed\n";
        return 1;
    }
    // 输出
    std::cout << "SO_SNDBUF: " << sb / 1024 << std::endl;
}
