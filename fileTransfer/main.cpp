#include "log.h"
#include "simple.h"
#include <algorithm>

int write_response(int sock, int length, int read_fd, TYPE type) {
    // 写入命令长度
    int ret = write_command_length(sock, length);
    if (ret < 0) {
        Perrorf("write length error");
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    // 写type
    write(sock, &type, sizeof(type));
    DEBUG << "length:[" << length << "] type[" << TYPE_STR[type] << "]";
    if (length == 1) {
        return 0;
    }
    // 读取文件内容
    char buf[4096] = {0};
    int readSize = 0;
    while (readSize < length) {
        int maxReadSize = std::min(int(length - readSize), int(sizeof(buf) - 1));
        DEBUG << "maxReadSize:" << maxReadSize;
        int ret = readn(read_fd, buf, maxReadSize);
        if (ret < 0) {
            ERROR << "read file error";
            return -1;
        }
        if (ret == 0) {
            Perrorf("read file error");
            DEBUG << "total[" << length << "] size[" << ret << "] ac["
                  << readSize << "] write:";
            break;
        }
        readSize += ret;
        ret = writen(sock, buf, ret);
        if (ret < 0) {
            ERROR << "write error";
            return -1;
        } else if (ret == 0) {
            WARN << "socket quit";
            return 0;
        }
        buf[ret] = '\0';
        DEBUG << "total[" << length << "] size[" << ret << "] ac[" << readSize
              << "] write:" << buf;
    }
    return readSize;
}

int check_dir_valid(const char* path) {
    if (path == NULL) {
        return -1;
    }
    struct stat st;
    int ret = stat(path, &st);
    if (ret < 0) {
        Perrorf("stat error");
        return -1;
    }
    if (!S_ISDIR(st.st_mode)) {
        return -1;
    }
    return 0;
}

char inner_buffer[4096];

int handle_client(int sock) {
    char buf[4096] = {0};
    const char* baseDir = "/Users/nichao/";
    if (check_dir_valid(baseDir) < 0) {
        ERROR << "baseDir invalid";
        return -1;
    }
    while (1) {
        command_t* cmd = NULL;
        command_guard guard(cmd);
        int ret = read_command(sock, &cmd);
        if (ret < 0) {
            WARN << "read command error";
            break;
        } else if (ret == 0) {
            WARN << "socket quit";
            break;
        }
        DEBUG << "read command success";
        // 输出读入的命令
        print_command(cmd);
        TYPE type = command_type(cmd);
        switch (type) {
            case LIST: {
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
                write_response(sock, len, pfd, LIST);
                pclose(fp);
                INFO << "send list success";
                break;
            }
            case READ: {
                DEBUG << "read";
                int len = cmd->size - sizeof(TYPE);
                std::string filename(command_inner_data(cmd),
                                     command_inner_data(cmd) + len);
                char path[4096] = {0};
                snprintf(path, sizeof(path), "%s%s", baseDir, filename.c_str());
                DEBUG << "path:" << path;
                FILE* fp = fopen(path, "r");
                if (fp == NULL) {
                    Perrorf("fopen error");
                    // 发送文件不存在
                    write_response(sock, 1, 0, READ);
                    break;
                }
                if (setvbuf(fp, inner_buffer, _IOFBF, sizeof(inner_buffer)) != 0) {
                    perror("Failed to set buffer");
                    fclose(fp);
                    return 1;
                }
                // 发送文件长度
                fseek(fp, 0, SEEK_END);
                int fileLen = ftell(fp);
                // 重置文件指针
                fseek(fp, 0, SEEK_SET);
                int fpd = fileno(fp);
                write_response(sock, fileLen, fpd, READ);
                fclose(fp);
                printf("send file success\n");
                break;
            }
            default: {
                INFO << "unknown command";
                break;
            }
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

    int status = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&ret,
                            sizeof(ret));
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
        if (inet_ntop(AF_INET, &client_addr.sin_addr, addr_buf,
                      sizeof(addr_buf)) == NULL) {
            std::cout << "inet_ntop error" << std::endl;
            continue;
        }
        INFO << "client connect:" << addr_buf << ":"
             << ntohs(client_addr.sin_port);
        handle_client(client_sock);
        close(client_sock);
    }
    close(sock);
}