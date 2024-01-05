#include "simple.h"
#include "log.h"

int read_client_command(int sock, char* buf, int len) {
    int ret = 0;
    // read length;
    int length = 0;
    int readSize = readn(sock, (char*)&length, sizeof(length));
    if (readSize < 0) {
        std::cout << "read length error" << std::endl;
        return -1;
    }
    // 小端
    length = ntohl(length);
    if (length > len) {
        std::cout << "command too long" << std::endl;
        return -1;
    }
    // read command
    readSize = readn(sock, buf, length);
    if (readSize < 0) {
        std::cout << "read command error" << std::endl;
        return -1;
    }
    return readSize;
}

void handle_client(int sock) {
    char buf[4096] = {0};
    const char* baseDir = "/home/homework/";
    while (1) {
        printf("wait client input: \n");
        int readSize = read_client_command(sock, buf, sizeof(buf));
        if (readSize < 0) {
            std::cout << "read client command error" << std::endl;
            break;
        }
        buf[readSize] = '\0';
        std::cout << "client command:(" << buf << ")" << std::endl;
        // 执行命令
        if (strncasecmp(buf, "quit", 4) == 0) {
            std::cout << "client quit" << std::endl;
            break;
        }
        if (strncasecmp(buf, "list", 4) == 0) {
            std::cout << "list" << std::endl;
            char cmd[4096] = {0};
            //
            snprintf(cmd, sizeof(cmd), "ls -l %s | wc -c", baseDir);
            FILE* fp = popen(cmd, "r");
            if (fp == NULL) {
                perror("popen error");
                break;
            }
            int len = 0;
            fscanf(fp, "%d", &len);
            pclose(fp);
            snprintf(cmd, sizeof(cmd), "ls -l %s", baseDir);
            fp = popen(cmd, "r");
            if (fp == NULL) {
                perror("popen error");
                break;
            }
            int pfp = fileno(fp);
            char* data = (char*)malloc(len + 1);
            int readSize = readn(pfp, data, len);
            if (readSize < 0) {
                WARN << "read error";
                break;
            }
            data[len] = '\0';
            command_t * read_cmd = new_command(LIST, len);
            command_set_data(read_cmd, (const char*)data, len);
            write_command(sock, read_cmd);
            INFO << "send list success";
        } else if (strncasecmp(buf, "read", 4) == 0) {
            const char* filename = buf + 4;
            std::cout << "read" << filename << std::endl;
            char path[4096] = {0};
            while (filename[0] == ' ') {
                filename++;
            }
            if (filename[0] == '\0') {
                std::cout << "filename error" << std::endl;
                continue;
            }
            snprintf(path, sizeof(path), "%s%s", baseDir, filename);

            FILE* fp = fopen(path, "r");
            if (fp == NULL) {
                std::cout << "file not exist" << path << std::endl;
                Perrorf("fopen error");
                // 发送文件不存在
                int length = htonl(0);
                memcpy(buf, &length, sizeof(length));
                write(sock, buf, length + sizeof(length));
                continue;
            }
            // 发送文件长度
            fseek(fp, 0, SEEK_END);
            int len = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            int length = htonl(len);
            printf("file length:%d\n", len);
            write(sock, (char*)&length, sizeof(length));
            // 发送文件内容
            while (fgets(buf, sizeof(buf), fp) != NULL) {
                int ret = write(sock, buf,  strlen(buf));
                buf[strlen(buf)] = '\0';
                printf("ret:%d send:%s", ret, buf);
            }
            fclose(fp);
            printf("send file success\n");
        } else {
            std::cout << "unknown command(" << buf << ")" << std::endl;
            break;
        }
    }
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        Perrorf("socket error");
        return -1;
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;
    int ret = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        Perrorf("bind error");
        return -1;
    }
    int status = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&ret, sizeof(ret));
    if (status < 0) {
        Perrorf("setsockopt error");
        return -1;
    }
    // 设置端口快速回收
    ret = listen(sock, 5);
    if (ret < 0) {
        Perrorf("listen error");
        return -1;
    }
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_sock = accept(sock, (struct sockaddr*)&client_addr, &len);
        if (client_sock < 0) {
            Perrorf("accept error");
            continue;
        }
        char addr_buf[INET_ADDRSTRLEN] = {0};
        if (inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf, sizeof(addr_buf)) == NULL) {
            std::cout << "inet_ntop error" << std::endl;
            continue;
        }
        std::cout << "client ip: " << addr_buf << ", port: " << ntohs(client_addr.sin_port) << std::endl;
        handle_client(client_sock);
        close(client_sock);
    }
    close(sock);
}