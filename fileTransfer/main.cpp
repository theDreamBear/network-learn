#include "log.h"
#include "simple.h"
#include <algorithm>

int read_from_file(FILE* fp, char* buf, int len) {
    int readSize = 0;
    while (readSize < len) {
        int ret = fread(buf + readSize, 1, len - readSize, fp);
        if (ret < 0) {
            Perrorf("read error");
            return -1;
        } else if (ret == 0) {
            DEBUG << "readSize:" << readSize;
            break;
        }
        readSize += ret;
    }
    return readSize;
}

/**
 * @brief 写命令
 * @param sock socket
 * @param cmd 命令
 * @return int
 * @retval -1 读取失败
 * @retval 0 socket关闭
 * @retval >0 写入的字节数
*/
int write_response(int sock, int length, FILE* fp, TYPE type) {
    command_t* cmd = NULL;
    command_guard guard(cmd);
    cmd = new_command(type, length);
    if (length > 0) {
        int ret = read_from_file(fp, command_inner_data(cmd), length);
        if (ret < 0) {
            ERROR << "read file error";
            return -1;
        } else if (ret == 0) {
            ERROR << "read file EOF";
            return 0;
        }
    }
    int ret = write_command(sock, cmd);
    if (ret < 0) {
        ERROR << "write command error";
        return -1;
    } else if (ret == 0) {
        WARN << "socket quit";
        return 0;
    }
    return ret;
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
    const char* baseDir = "/home/homework/";
    if (check_dir_valid(baseDir) < 0) {
        ERROR << "baseDir invalid";
        abort();
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
                write_response(sock, len, fp, LIST);
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
                    const char* msg = "file not exist";
                    FILE* fp = fmemopen((void*)msg, strlen(msg), "r");
                    if (fp == NULL) {
                        Perrorf("fmemopen error");
                        return -1;
                    }
                    ret = write_response(sock, strlen(msg), fp, ERR);
                    if (ret < 0) {
                        ERROR << "write error";
                        return -1;
                    } else if (ret == 0) {
                        WARN << "socket quit";
                        return 0;
                    }
                    fclose(fp);
                    break;
                }
                // 发送文件长度
                fseek(fp, 0, SEEK_END);
                int fileLen = ftell(fp);
                // 重置文件指针
                fseek(fp, 0, SEEK_SET);
                write_response(sock, fileLen, fp, READ);
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