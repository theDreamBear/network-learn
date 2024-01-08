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

int write_response(int sock, int length, int read_fd) {
    int nlen = htonl(length);
    int ret = writen(sock, (char*)&nlen, sizeof(nlen));
    if (ret < 0) {
        std::cout << "write length error" << std::endl;
        return -1;
    }
    char buf[4096] = {0};
    int readSize = 0;
    while (readSize < length) {
        int maxReadSize = sizeof(buf) - 1 > length - readSize ? length - readSize : sizeof(buf) - 1;
        int ret = readn(read_fd, buf, maxReadSize);
        if (ret < 0) {
            ERROR << "read file error";
            return -1;
        }
        if (ret == 0) {
            WARN << "fd close";
            return -1;
        }
        readSize += ret;
        ret = writen(sock, buf, ret);
        if (ret < 0) {
            ERROR << "write error";
            return -1;
        }
        buf[ret] = '\0';
        DEBUG << "size:" << ret << " ac:" << readSize  << " write:" << buf;
    }
    return readSize;
}

void handle_client(int sock) {
    char buf[4096] = {0};
    const char* baseDir = "/Users/nichao/";
    while (1) {
        command_t* cmd = NULL;
        int ret = read_command(sock, &cmd);
        command_guard guard(cmd);
        if (ret < 0) {
            WARN << "read command error";
            break;
        }
        if (ret == 0) {
            WARN << "client quit";
            break;
        }
        if (cmd == NULL) {
            WARN << "command is null";
            continue;
        }
        if (command_type(cmd) == LIST) {
            DEBUG << "list";
            char cmd[1024] = {0};
            snprintf(cmd, sizeof(cmd), "ls -l %s | wc -c", baseDir);
            DEBUG << "cmd:" << cmd;
            FILE* fp = popen(cmd, "r");
            if (fp == NULL) {
                perror("popen error");
                break;
            }
            int len = 0;
            fscanf(fp, "%d", &len);
            pclose(fp);
            DEBUG << "list length:" << len;
            snprintf(cmd, sizeof(cmd), "ls -l %s", baseDir);
            fp = popen(cmd, "r");
            if (fp == NULL) {
                perror("popen error");
                break;
            }
            int pfd = fileno(fp);
            write_response(sock, len, pfd);
            pclose(fp);
            INFO << "send list success";
        } else if (command_type(cmd) == READ) {
            DEBUG << "read";
            int len = cmd->size - sizeof(TYPE);
            std::string filename(command_inner_data(cmd), command_inner_data(cmd) + len);
            char path[4096] = {0};
            snprintf(path, sizeof(path), "%s%s", baseDir, filename.c_str());
            DEBUG << "path:" << path;
            FILE* fp = fopen(path, "r");
            if (fp == NULL) {
                Perrorf("fopen error");
                // 发送文件不存在
                int length = htonl(0);
                memcpy(buf, &length, sizeof(length));
                write(sock, buf, sizeof(length));
                continue;
            }
            // 发送文件长度
            fseek(fp, 0, SEEK_END);
            int fileLen = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            int fpd = fileno(fp);
            write_response(sock, fileLen, fpd);
            fclose(fp);
            printf("send file success\n");
        } else {
            std::cout << "unknown command(" << buf << ")" << std::endl;
            break;
        }
    }
}

int main() {
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    int sock;
    Socket(sock);
    Bind(sock, addr);
    int ret = 1;

    int status = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&ret, sizeof(ret));
    if (status < 0) {
        Perrorf("setsockopt error");
        return -1;
    }
    Listen(sock);
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client_sock;
        Accept(sock, client_sock, client_addr);
        char addr_buf[INET_ADDRSTRLEN] = {0};
        if (inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf, sizeof(addr_buf)) == NULL) {
            std::cout << "inet_ntop error" << std::endl;
            continue;
        }
        INFO << "client connect:" << addr_buf << ":" << ntohs(client_addr.sin_port);
        handle_client(client_sock);
        close(client_sock);
    }
    close(sock);
}