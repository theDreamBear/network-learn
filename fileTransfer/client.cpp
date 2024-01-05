#include "simple.h"
#include <string>

int input_command(command_t** cmd, int sock) {
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
            command_set_data(*cmd, filename.c_str(), filename.size());
        } else {
            INFO << "unknown command" << std::endl;
            ret = -1;
        }
    }
    print_command(*cmd);
    return ret;
}

int main(int argc, char** argv) {
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("10.116.32.38");

    int sock;
    Socket(sock);
    Connect(sock, addr);
    char buf[4096] = {0};
    while (1) {
        command_t* cmd = NULL;
        command_guard guard(cmd);
        int ret = input_command(&cmd, sock);
        if (command_type(cmd) == QUIT) {
            INFO << "quit";
            break;
        }
        if (cmd == NULL) {
            WARN << "command is null";
            continue;
        }
        if (ret < 0) {
            WARN << "input command error";
            continue;
        }
        ret = write_command(sock, cmd);
        if (ret < 0) {
            ERROR << "write command error";
            break;
        }
        DEBUG << "write command success";
        command_t* read_cmd = NULL;
        ret = read_command(sock, &read_cmd);
        if (ret < 0) {
            ERROR << "read command error";
        }
        if (ret == 0) {
            break;
        }
        print_command(read_cmd);
        if (command_type(read_cmd) == READ) {
            int len = cmd->size - sizeof(TYPE);
            std::string filename(command_inner_data(cmd), command_inner_data(cmd) + len);
            FILE* fp = fopen(filename.c_str(), "w");
            if (fp == NULL) {
                Perrorf("fopen error");
                break;
            }
            fwrite(command_data(read_cmd), 1, read_cmd->size, fp);
            fclose(fp);
            INFO << "write file success";
        }
    }
    close(sock);
}