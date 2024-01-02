#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    auto sd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sd) {
        perror("socket");
        return 1;
    }
    // 初始化
    struct sockaddr_in server;
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(1234);
    // 绑定
    if (-1 == bind(sd, (struct sockaddr*)&server, sizeof(server))) {
        perror("bind");
        return 1;
    }
    // 监听
    if (-1 == listen(sd, 5)) {
        perror("listen");
        return 1;
    }
    // 接受连接
    while(1) {
        // 接受客户端连接
        struct sockaddr_in client;
        socklen_t len = sizeof(client);
        auto conn = accept(sd, (struct sockaddr *) &client, &len);
        if (-1 == conn) {
            perror("accept");
            continue;
        }
        // 打印客户端信息
        std::cout << "client ip: " << inet_ntoa(client.sin_addr) << ", port: " << ntohs(client.sin_port) << std::endl;
        // 读取客户端数据
        char buf[1024];
        auto n = read(conn, buf, sizeof(buf));
        if (-1 == n) {
            perror("read");
            continue;
        }
        n = std::min(1023, int(n));
        buf[n] = '\0';
        std::cout << "read: " << buf << std::endl;
        // 发送数据
        const char *msg = "hello, client";
        n = write(conn, msg, strlen(msg));
        if (-1 == n) {
            perror("write");
        }
        // 关闭连接
        close(conn);
    }
}
