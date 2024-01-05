#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <string>
#include <stdio.h>
#include <unistd.h>

#define Perrorf(str) { \
    char buffer[1024] = {0}; \
    snprintf(buffer, sizeof(buffer), "%s line[%d], func[%s]", str, __LINE__, __func__); \
    perror(buffer); \
}

int readn(int sock, char* buf, int len) {
    int readSize = 0;
    while (readSize < len) {
        int ret = read(sock, buf + readSize, len - readSize);
        if (ret < 0) {
            Perrorf("read error");
            return -1;
        }
        readSize += ret;
    }
    return readSize;
}

int strip(char* buf, int len) {
    int i = 0;
    while (i < len && buf[i] == ' ') {
        ++i;
    }
    return i;
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
    addr.sin_addr.s_addr = inet_addr("10.116.32.38");
    int ret = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        Perrorf("connect error");
        return -1;
    }
    char buf[4096] = {0};
    while (1) {
        std::cout << "please input command: ";
        std:: string cmd, filename;
        std::cin.getline(buf, sizeof(buf));
        int len = strlen(buf);
        buf[len] = '\0';
        if (len == 0) {
            continue;
        }
        if (strstr(buf, "read") == buf) {
            cmd = "read";
            filename = buf + 4;
            while (filename[0] == ' ') {
                filename = filename.substr(1);
            }
        } else if (strstr(buf, "quit") == buf) {
            break;
        } else if (strstr(buf, "list") == buf) {
            cmd = "list";
        } else {
            std::cout << "command error" << buf << std::endl;
            continue;
        }
        // 发送命令
        printf("cmd:%s\n", buf);
        // 发送命令长度
        int length = htonl(len);
        write(sock, (char*)&length, sizeof(length));
        // 发送命令
        write(sock, buf, len);
        // 接收结果
        // 命令长度
        length = 0;
        int rd = readn(sock, (char*)&length, sizeof(length));
        if (rd < 0) {
            std::cout << "read error" << std::endl;
            return -1;
        }
        if (cmd == "list") {
            // 命令结果
            int len = ntohl(length);
            char buffer[4096] = {0};
            while (len > 0) {
                    rd = readn(sock, buffer, len);
                    if (rd < 0) {
                        Perrorf("read error");
                        return -1;
                    }
                    buffer[rd] = '\0';
                    std::cout << "len:" << len << "\t" << buffer;

                int rd = readn(sock, (char*)&length, sizeof(length));
                if (rd < 0) {
                    Perrorf("read error");
                    return -1;
                }
                len = ntohl(length);
            }
            continue;
        } else if (cmd == "read") {
            // 命令结果
            int len = ntohl(length);
            FILE* fp = fopen(filename.c_str(), "w");
            if (fp == NULL) {
                Perrorf("fopen error");
                break;
            }
            printf("len:%d\n", len);
            int rd = 0;
            while (rd < len) {
                int one_len = std::min((int)sizeof(buf + 1), len - rd);
                int ret = readn(sock, buf, one_len);
                if (ret < 0) {
                    std::cout << "read error" << std::endl;
                    break;
                }
                fwrite(buf, 1, ret, fp);
                rd += ret;
                fflush(fp);
                buf[ret] = '\0';
                printf("rd:%d %s\n", rd, buf);
            }
            fclose(fp);
            printf("read file[%s] success\n", filename.c_str());
        } else {
            std::cout << "unknown command" << std::endl;
            continue;
        }
    }
    close(sock);
}