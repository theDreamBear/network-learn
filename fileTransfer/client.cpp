#include "simple.h"
#include <string>

/**
 * @brief 从标准输入读取命令
 * @param cmd 命令
 * @param sock socket
 * @return int
 * @retval 0 成功， 成功的时候cmd不为NULL
 * @retval -1 失败， 失败时cmd为NULL
 * @note 读取的命令需要手动释放
 *
*/
int input_command( int sock, command_t** cmd) {
    printf("client[%d]>> ", sock);
    std::string cmd_name;
    std::cin >> cmd_name;
    TYPE type = QUIT;
    if (cmd_name == "quit") {
        type = QUIT;
    } else if (cmd_name == "list") {
        type = LIST;
    } else if (cmd_name == "read") {
        type = READ;
    } else {
        INFO << "unknown command" << std::endl;
        return -1;
    }
    *cmd = new_command(type, 0);
    int ret = 0;
    if (has_data(*cmd)) {
        if (type == READ) {
            std::string filename;
            std::cin >> filename;
            DEBUG << "filename[" << filename << "]" << std::endl;
            set_command_body(*cmd, filename.c_str(), filename.size());
        } else {
            INFO << "unknown command" << std::endl;
            ret = -1;
        }
    } else {
        DEBUG << "TYPE:" << TYPE_STR[type];
    }
    print_command((*cmd));
    return ret;
}

int main(int argc, char** argv) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int sock;
    Socket(sock);
    Connect(sock, addr);
    char buf[4096] = {0};
    while (1) {
        // 从命令行读取命令
        command_t* cmd = NULL;
        command_guard guard(cmd);
        int ret = input_command(sock, &cmd);
        if (ret < 0) {
            WARN << "input command error";
            continue;
        }

        if (command_type(cmd) == QUIT) {
            INFO << "quit";
            break;
        }
        // 发送命令
        ret = write_command(sock, cmd);
        if (ret < 0) {
            ERROR << "write command error";
            break;
        } else if (ret == 0) {
            INFO << "socket quit";
            break;
        }
        print_command(cmd);
        // 从服务端读取命令
        command_t* read_cmd = NULL;
        command_guard read_guard(read_cmd);
        ret = read_command(sock, &read_cmd);
        if (ret < 0) {
            ERROR << "read command error";
        }
        if (ret == 0) {
            break;
        }
        print_command(read_cmd);
        TYPE type = command_type(read_cmd);
        switch (type) {
            case READ: {
                if (read_cmd->size == 1) {
                    INFO << "file not exist";
                    break;
                }
                int len = read_cmd->size - sizeof(TYPE);
                std::string filename(command_inner_data(cmd), command_inner_data(cmd) + len);
                FILE* fp = fopen(filename.c_str(), "w");
                if (fp == NULL) {
                    Perrorf("fopen error");
                    break;
                }
                fwrite(command_inner_data(read_cmd), 1, read_cmd->size - sizeof(TYPE), fp);
                fclose(fp);
                INFO << "write file success";
                break;
            }
            case LIST: {
                INFO << "list";
                break;
            }
            default: {
                INFO << "unknown command";
                break;
            }
        }
    }
    close(sock);
}